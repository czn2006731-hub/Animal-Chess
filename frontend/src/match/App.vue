<script setup lang="ts">
import { computed, onMounted, onUnmounted, ref } from 'vue'
import Brand from '../shared/Brand.vue'
import { api } from '../shared/api'
import type { RankItem, UserProfile } from '../shared/types'

const profile = ref<UserProfile>({ id: 0, username: '棋手', score: 0, level: 1, wins: 0, losses: 0, draws: 0 })
const ranking = ref<RankItem[]>([])
const busy = ref(false)
const privateRoom = ref('')
const showRules = ref(false)
const feedback = ref('')
let pollTimer: number | undefined

const winRate = computed(() => {
  const total = profile.value.wins + profile.value.losses + profile.value.draws
  return total ? `${Math.round((profile.value.wins / total) * 100)}%` : '—'
})

async function loadLobby() {
  try {
    const [me, ranks] = await Promise.all([api<UserProfile>('/api/me'), api<RankItem[]>('/api/ranking?limit=50')])
    if (me.ok && me.data) profile.value = me.data
    if (ranks.ok && ranks.data) ranking.value = ranks.data
  } catch {
    feedback.value = '暂时无法连接服务端，界面仍可预览。'
  }
}

async function randomMatch() {
  busy.value = true
  feedback.value = '正在寻找势均力敌的对手…'
  try {
    const result = await api<{ roomId?: string }>('/api/match/join', { method: 'POST' })
    if (!result.ok) throw new Error(result.message ?? '加入匹配失败。')
    pollTimer = window.setInterval(checkMatch, 1000)
  } catch (cause) {
    feedback.value = cause instanceof Error ? cause.message : '匹配服务不可用。'
    busy.value = false
  }
}

async function checkMatch() {
  try {
    const result = await api<{ matched: boolean; roomId?: string }>('/api/match/status')
    if (result.data?.matched && result.data.roomId) openRoom(result.data.roomId)
  } catch { /* Preserve the waiting state until the user cancels. */ }
}

async function cancelMatch() {
  await api('/api/match/cancel', { method: 'POST' }).catch(() => undefined)
  if (pollTimer) window.clearInterval(pollTimer)
  pollTimer = undefined
  busy.value = false
  feedback.value = '已取消匹配。'
}

async function createPrivateRoom() {
  try {
    const result = await api<{ roomId: string }>('/api/private-room/create', { method: 'POST' })
    if (!result.ok || !result.data) throw new Error(result.message ?? '创建房间失败。')
    feedback.value = `房间号 ${result.data.roomId} 已创建，等待好友加入。`
    openRoom(result.data.roomId)
  } catch (cause) { feedback.value = cause instanceof Error ? cause.message : '创建房间失败。' }
}

async function joinPrivateRoom() {
  const roomId = privateRoom.value.trim().toUpperCase()
  if (!roomId) return void (feedback.value = '请输入好友提供的房间号。')
  try {
    const result = await api<{ roomId: string }>('/api/private-room/join', { method: 'POST', body: JSON.stringify({ roomId }) })
    if (!result.ok || !result.data) throw new Error(result.message ?? '无法加入该房间。')
    openRoom(result.data.roomId)
  } catch (cause) { feedback.value = cause instanceof Error ? cause.message : '无法加入该房间。' }
}

function openRoom(roomId: string) { window.location.assign(`/room.html?room=${encodeURIComponent(roomId)}`) }
onMounted(loadLobby)
onUnmounted(() => { if (pollTimer) window.clearInterval(pollTimer) })
</script>

<template>
  <main class="lobby-page">
    <header class="shell topbar"><Brand /><div class="player-nav"><span class="pill">Lv. {{ profile.level }}</span><strong>{{ profile.username }}</strong><a href="/login.html">退出</a></div></header>
    <section class="shell lobby-grid">
      <aside class="side-column">
        <section class="profile-card card">
          <div class="avatar">{{ profile.username.slice(0, 1).toUpperCase() }}</div>
          <div><span class="eyebrow">我的棋手档案</span><h2>{{ profile.username }}</h2><p><b>{{ profile.score }}</b> 积分 · Lv. {{ profile.level }}</p></div>
          <dl><div><dt>胜场</dt><dd>{{ profile.wins }}</dd></div><div><dt>负场</dt><dd>{{ profile.losses }}</dd></div><div><dt>胜率</dt><dd>{{ winRate }}</dd></div></dl>
        </section>
        <section class="records card"><div class="section-head"><h2>最近战绩</h2><span>对局结束后更新</span></div><div class="empty-record">完成第一盘对局后，在这里查看最近战绩。</div></section>
      </aside>

      <section class="action-column">
        <div class="intro"><p class="eyebrow">匹配大厅</p><h1>下一盘，<em>谁来应战？</em></h1><p>标准规则 · 胜 +4 分 · 负 −4 分 · 每步 30 秒</p></div>
        <section class="match-card card">
          <div class="match-symbol" aria-hidden="true"><span>兽</span><i>VS</i><span>兽</span></div>
          <h2>{{ busy ? '正在寻找对手' : '随机匹配' }}</h2>
          <p>{{ busy ? '请保持页面打开，找到对手后将自动进入房间。' : '系统会为你寻找一名正在等待的棋手。' }}</p>
          <button v-if="!busy" class="button button--blue primary-action" @click="randomMatch">开始匹配</button>
          <button v-else class="button button--quiet primary-action" @click="cancelMatch">取消匹配</button>
        </section>
        <section class="private-card card">
          <div><p class="eyebrow">和朋友一起玩</p><h2>私人房间</h2><p>创建房间号，或输入好友分享的号码加入。</p></div>
          <div class="private-actions"><button class="button button--quiet" @click="createPrivateRoom">创建房间</button><div><label class="sr-only" for="room-code">房间号</label><input id="room-code" v-model="privateRoom" class="input" maxlength="12" placeholder="输入房间号" @keyup.enter="joinPrivateRoom" /><button class="button" @click="joinPrivateRoom">加入</button></div></div>
        </section>
        <p v-if="feedback" class="lobby-feedback" role="status">{{ feedback }}</p>
        <button class="rules-link" @click="showRules = !showRules">{{ showRules ? '收起规则' : '查看标准斗兽棋规则' }} <span aria-hidden="true">↓</span></button>
        <section v-if="showRules" class="rules card"><b>走兽大小：</b>象 ＞ 狮 ＞ 虎 ＞ 豹 ＞ 狼 ＞ 狗 ＞ 猫 ＞ 鼠。鼠可下水，狮虎可跃河；进入对方兽穴即获胜，落入对方陷阱后会失去战斗力。</section>
      </section>

      <aside class="rank-card card"><div class="section-head"><div><p class="eyebrow">排行榜</p><h2>本周棋手</h2></div><span>积分</span></div><ol v-if="ranking.length"><li v-for="item in ranking" :key="item.id"><b :class="{ podium: item.rank <= 3 }">{{ item.rank }}</b><span>{{ item.username }}<small>胜率 {{ item.winRate }}%</small></span><strong>{{ item.score }}</strong></li></ol><div v-else class="empty-rank">排行榜将在服务端启动后显示。</div></aside>
    </section>
  </main>
</template>

