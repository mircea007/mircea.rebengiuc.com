// since this script cannot use jquery (because it's job is to import it)
// that means we will have to do this in vanila js

function addCSS( url, type = "text/css" ){
  console.log(`loading css: ${url}`);
  var el = document.createElement("link");
  el.setAttribute("rel", "stylesheet");
  el.setAttribute("type", type);
  el.setAttribute("href", url);
  document.getElementsByTagName("head")[0].appendChild(el);
}

function addJS( url ){
  console.log(`loading js: ${url}`);
  var el = document.createElement("script");
  el.setAttribute("src", url);
  document.getElementsByTagName("head")[0].appendChild(el);
}

function wait_for( condition, callback ){
  if( condition() )
    callback();
  else
    setTimeout(_ => wait_for(condition, callback), 50);
}

const root_prefix = "/";

// external

addJS("https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"); // google jquery CDN
addCSS(root_prefix + "css/tailwind.css");  // compiled tailwind stylesheet

if( false ){ // tailwind play cdn
  addJS("https://cdn.jsdelivr.net/npm/@tailwindcss/browser@4");
  addCSS(root_prefix + "css/main.css", "text/tailwindcss");
}

// internal

wait_for(_ => window.jQuery, _ => {
  addJS(root_prefix + "js/load-templates.js");     // loads templates

  wait_for(_ => ($('.navbar').length > 0), _ => {
    const url = new URL(window.location.href);
    $(`div.navbar div.flex a[href="${url.pathname}"]`).each(function(){ $(this).addClass('current'); });
    console.log('highlighted current');

    document.body.style = '';
    console.log('displayed page');
  });
});
