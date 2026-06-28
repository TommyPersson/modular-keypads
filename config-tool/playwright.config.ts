import { defineConfig } from '@playwright/test';

export default defineConfig({
  webServer: {
    port: 9900,
    command: 'npm run dev:playwright',
  },
  testDir: './tests',
});