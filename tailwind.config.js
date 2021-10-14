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
  },
  variants: {
    extend: {
      height: ['hover']
    },
  },
  plugins: [],
}
