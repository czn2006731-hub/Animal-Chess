<script setup lang="ts">
import { ref } from 'vue'
import Brand from '../shared/Brand.vue'
import { api } from '../shared/api'

const mode = ref<'login' | 'register'>('login')
const username = ref('')
const password = ref('')
const confirmPassword = ref('')
const loading = ref(false)
const error = ref('')

async function submit() {
  error.value = ''
  if (username.value.trim().length < 3) return void (error.value = '用户名需为 3–20 位字母、数字或下划线。')
  if (password.value.length < 8) return void (error.value = '密码至少需要 8 位。')
  if (mode.value === 'register' && password.value !== confirmPassword.value) return void (error.value = '两次输入的密码不一致。')

  loading.value = true
  try {
    const result = await api<unknown>(`/api/auth/${mode.value}`, { method: 'POST', body: JSON.stringify({ username: username.value.trim(), password: password.value }) })
    if (!result.ok) throw new Error(result.message ?? '请求未完成，请稍后重试。')
    window.location.assign('/match.html')
  } catch (cause) {
    error.value = cause instanceof Error ? cause.message : '网络连接异常。'
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <main class="login-page">
    <header class="shell topbar"><Brand /><span class="pill"><span class="status-dot" />在线对战</span></header>
    <section class="shell login-layout">
      <div class="hero-copy">
        <p class="eyebrow">标准斗兽棋 · 在线对战</p>
        <h1 class="title">用一步判断，<br /><em>赢下整盘棋。</em></h1>
        <p class="subtitle">随机匹配、私人房间与实时对话。每一步都由服务器校验，公平地决定胜负。</p>
        <div class="feature-list" aria-label="游戏特色">
          <span>标准规则</span><span>30 秒落子</span><span>10 秒重连</span>
        </div>
        <div class="animal-row" aria-hidden="true"><i>鼠</i><i>猫</i><i>虎</i><i>象</i></div>
      </div>
      <section class="auth-card card" aria-label="账户操作">
        <div class="auth-switch" role="tablist" aria-label="登录或注册">
          <button :class="{ active: mode === 'login' }" role="tab" :aria-selected="mode === 'login'" @click="mode = 'login'; error = ''">登录</button>
          <button :class="{ active: mode === 'register' }" role="tab" :aria-selected="mode === 'register'" @click="mode = 'register'; error = ''">注册</button>
        </div>
        <form @submit.prevent="submit">
          <h2>{{ mode === 'login' ? '欢迎回来' : '创建你的棋手档案' }}</h2>
          <p>使用用户名与密码进入匹配大厅。</p>
          <label>用户名<input v-model="username" class="input" autocomplete="username" maxlength="20" placeholder="例如 animal_player" /></label>
          <label>密码<input v-model="password" class="input" type="password" autocomplete="current-password" placeholder="至少 8 位" /></label>
          <label v-if="mode === 'register'">确认密码<input v-model="confirmPassword" class="input" type="password" autocomplete="new-password" placeholder="再次输入密码" /></label>
          <p v-if="error" class="notice" role="alert">{{ error }}</p>
          <button class="button submit" :disabled="loading" type="submit">{{ loading ? '处理中…' : mode === 'login' ? '进入匹配大厅' : '创建账号并进入' }}</button>
        </form>
      </section>
    </section>
  </main>
</template>

