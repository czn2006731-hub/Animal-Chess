<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref } from 'vue'
import Brand from '../shared/Brand.vue'
import { webSocketUrl } from '../shared/api'
import type { Side } from '../shared/types'

type Piece = { side: Side; rank: number; label: string }
type Cell = Piece | null
type Message = { from: string; text: string; own?: boolean; system?: boolean }

const roomId = new URLSearchParams(window.location.search).get('room') ?? '预览房间'
const board = ref<Cell[][]>(initialBoard())
const mySide = ref<Side>('red')
const turn = ref<Side>('red')
const selected = ref<[number, number] | null>(null)
const seconds = ref(30)
const connected = ref(false)
const connectionText = ref('正在连接房间…')
const chatInput = ref('')
const messages = ref<Message[]>([{ from: '系统', text: '对局即将开始，红方先行。', system: true }])
let socket: WebSocket | undefined
let timer: number | undefined

const isMyTurn = computed(() => turn.value === mySide.value)
const turnName = computed(() => turn.value === 'red' ? '红方' : '蓝方')

function initialBoard(): Cell[][] {
  const cells: Cell[][] = Array.from({ length: 9 }, () => Array.from({ length: 7 }, () => null))
  const put = (row: number, col: number, side: Side, rank: number, label: string) => { cells[row][col] = { side, rank, label } }
  put(0, 0, 'blue', 7, '狮'); put(0, 6, 'blue', 6, '虎'); put(1, 1, 'blue', 3, '狗'); put(1, 5, 'blue', 2, '猫'); put(2, 0, 'blue', 1, '鼠'); put(2, 2, 'blue', 5, '豹'); put(2, 4, 'blue', 4, '狼'); put(2, 6, 'blue', 8, '象')
  put(6, 0, 'red', 8, '象'); put(6, 2, 'red', 4, '狼'); put(6, 4, 'red', 5, '豹'); put(6, 6, 'red', 1, '鼠'); put(7, 1, 'red', 2, '猫'); put(7, 5, 'red', 3, '狗'); put(8, 0, 'red', 6, '虎'); put(8, 6, 'red', 7, '狮')
  return cells
}

function isWater(row: number, col: number) { return row >= 3 && row <= 5 && (col === 1 || col === 2 || col === 4 || col === 5) }
function isDen(row: number, col: number) { return (row === 0 || row === 8) && col === 3 }

function clickCell(row: number, col: number) {
  if (!isMyTurn.value) return
  const piece = board.value[row][col]
  if (!selected.value) {
    if (piece?.side === mySide.value) selected.value = [row, col]
    return
  }
  const [fromRow, fromCol] = selected.value
  if (piece?.side === mySide.value) return void (selected.value = [row, col])
  const payload = { type: 'game.move', from: [fromRow, fromCol], to: [row, col] }
  if (socket?.readyState === WebSocket.OPEN) socket.send(JSON.stringify(payload))
  else previewMove(fromRow, fromCol, row, col)
  selected.value = null
}

function previewMove(fromRow: number, fromCol: number, toRow: number, toCol: number) {
  const piece = board.value[fromRow][fromCol]
  if (!piece || Math.abs(fromRow - toRow) + Math.abs(fromCol - toCol) !== 1 || (isWater(toRow, toCol) && piece.rank !== 1) || isDen(toRow, toCol)) return
  board.value[toRow][toCol] = piece
  board.value[fromRow][fromCol] = null
  turn.value = turn.value === 'red' ? 'blue' : 'red'
  seconds.value = 30
}

function sendChat(text = chatInput.value) {
  const content = text.trim()
  if (!content) return
  if (socket?.readyState === WebSocket.OPEN) socket.send(JSON.stringify({ type: 'chat.text', content }))
  messages.value.push({ from: '我', text: content, own: true })
  chatInput.value = ''
}

function connect() {
  try {
    socket = new WebSocket(webSocketUrl())
    socket.onopen = () => { connected.value = true; connectionText.value = '已连接，等待房间确认'; socket?.send(JSON.stringify({ type: 'room.join', roomId })) }
    socket.onmessage = (event) => receive(JSON.parse(event.data))
    socket.onclose = () => { connected.value = false; connectionText.value = '连接断开，10 秒内可重连'; window.setTimeout(connect, 2000) }
    socket.onerror = () => { connectionText.value = '服务端尚未启动，当前为棋盘预览。'; socket?.close() }
  } catch { connectionText.value = '当前为棋盘预览。' }
}

function receive(message: Record<string, unknown>) {
  if (message.type === 'game.state' && Array.isArray(message.board)) { /* Server state hydration is connected in the next backend step. */ }
  if (message.type === 'chat.message') messages.value.push({ from: String(message.from ?? '对手'), text: String(message.content ?? '') })
  if (message.type === 'error') messages.value.push({ from: '系统', text: String(message.message ?? '操作未被接受。'), system: true })
}

onMounted(() => { connect(); timer = window.setInterval(() => { if (seconds.value > 0) seconds.value -= 1 }, 1000) })
onBeforeUnmount(() => { socket?.close(); if (timer) window.clearInterval(timer) })
</script>

<template>
  <main class="room-page">
    <header class="shell topbar room-topbar"><Brand /><div class="room-meta"><span>房间 {{ roomId }}</span><span :class="['connection', { offline: !connected }]"><i />{{ connectionText }}</span></div></header>
    <section class="shell battle-layout">
      <section class="game-card card">
        <div class="opponent-bar"><div class="opponent-avatar blue">对</div><div><strong>等待对手</strong><small>蓝方</small></div><div class="turn-clock" :class="{ active: turn === 'blue' }">{{ turn === 'blue' ? seconds : '—' }}<small>秒</small></div></div>
        <div class="turn-banner"><span :class="['side-dot', turn]" />{{ turnName }}回合 <b>{{ isMyTurn ? '轮到你落子' : '等待对手走棋' }}</b></div>
        <div class="board" role="grid" aria-label="斗兽棋棋盘">
          <button v-for="cellNumber in 63" :key="cellNumber" :class="['square', { water: isWater(Math.floor((cellNumber - 1) / 7), (cellNumber - 1) % 7), den: isDen(Math.floor((cellNumber - 1) / 7), (cellNumber - 1) % 7), selected: selected?.[0] === Math.floor((cellNumber - 1) / 7) && selected?.[1] === (cellNumber - 1) % 7 }]" :aria-label="`第 ${Math.floor((cellNumber - 1) / 7) + 1} 行第 ${(cellNumber - 1) % 7 + 1} 列`" @click="clickCell(Math.floor((cellNumber - 1) / 7), (cellNumber - 1) % 7)">
            <span v-if="board[Math.floor((cellNumber - 1) / 7)][(cellNumber - 1) % 7]" :class="['piece', board[Math.floor((cellNumber - 1) / 7)][(cellNumber - 1) % 7]?.side]">{{ board[Math.floor((cellNumber - 1) / 7)][(cellNumber - 1) % 7]?.label }}</span>
          </button>
        </div>
        <div class="me-bar"><div class="opponent-avatar red">我</div><div><strong>你</strong><small>红方 · Lv. 1</small></div><div class="turn-clock" :class="{ active: turn === 'red' }">{{ turn === 'red' ? seconds : '—' }}<small>秒</small></div></div>
      </section>
      <aside class="chat-card card">
        <div class="chat-head"><div><p class="eyebrow">实时对话</p><h2>房间聊天</h2></div><span class="pill">2 人</span></div>
        <div class="messages" aria-live="polite"><p v-for="(message, index) in messages" :key="index" :class="{ own: message.own, system: message.system }"><small v-if="!message.own && !message.system">{{ message.from }}</small><span>{{ message.text }}</span></p></div>
        <div class="quick-chat"><button v-for="phrase in ['走得漂亮', '轮到你了', '加油', '再来一局']" :key="phrase" @click="sendChat(phrase)">{{ phrase }}</button></div>
        <form class="chat-form" @submit.prevent="sendChat()"><label class="sr-only" for="chat">发送消息</label><input id="chat" v-model="chatInput" class="input" maxlength="100" placeholder="说点什么…" /><button class="button" type="submit">发送</button></form>
      </aside>
    </section>
  </main>
</template>

