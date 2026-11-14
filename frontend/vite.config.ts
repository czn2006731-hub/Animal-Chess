import { fileURLToPath, URL } from 'node:url'
import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  plugins: [vue()],
  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },
  build: {
    rollupOptions: {
      input: {
        login: fileURLToPath(new URL('./login.html', import.meta.url)),
        match: fileURLToPath(new URL('./match.html', import.meta.url)),
        room: fileURLToPath(new URL('./room.html', import.meta.url))
      }
    }
  }
})

