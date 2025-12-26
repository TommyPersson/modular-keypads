
export function takeLast<T>(arr: T[], count: number): T[] {
  return arr.length > count ? arr.slice(arr.length - count) : arr
}

export function takeFirst<T>(arr: T[], count: number): T[] {
  return arr.length > count ? arr.slice(0, count) : arr
}
