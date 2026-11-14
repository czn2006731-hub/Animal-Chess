export type Side = 'red' | 'blue'

export interface UserProfile {
  id: number
  username: string
  score: number
  level: number
  wins: number
  losses: number
  draws: number
}

export interface RankItem extends UserProfile {
  rank: number
  winRate: number
}

export interface ApiResult<T> {
  ok: boolean
  message?: string
  data?: T
}

