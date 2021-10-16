// if i ever need extra colors
const colors = require('tailwindcss/colors');

module.exports = {
  mode: 'jit',
  purge: [
    "./static/**/*.html",
    "./static/**/*.css",
    "./static/**/*.js"
  ],
  darkMode: false, // or 'media' or 'class'
  theme: {
    extend: {},
    fontFamily: {
      'sans': ["Inter var,ui-sans-serif,system-ui,-apple-system,BlinkMacSystemFont,Segoe UI,Roboto,Helvetica Neue,Arial,Noto Sans,sans-serif,Apple Color Emoji,Segoe UI Emoji,Segoe UI Symbol,Noto Color Emoji"]
    }
  },
  variants: {
    extend: {
      height: ['hover']
    },
  },
  plugins: [],
}
