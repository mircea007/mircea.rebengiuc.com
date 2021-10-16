// since this script cannot use jquery (because it's job is to import it)
// that means we will have to do this in vanila js so yeah, bloat time

function addCSS( url ){
  console.log("loading css: " + url);
  var el = document.createElement("link");
  el.setAttribute("rel", "stylesheet");
  el.setAttribute("type", "text/css");
  el.setAttribute("href", url);
  document.getElementsByTagName("head")[0].appendChild(el);
}

function addJS( url ){
  console.log("loading js: " + url);
  var el = document.createElement("script");
  el.setAttribute("src", url);
  document.getElementsByTagName("head")[0].appendChild(el);
}

function wait_for( condition, callback ){
  if( condition() )
    callback();
  else
    setTimeout(function(){wait_for(condition, callback)}, 50);
}

var root_prefix = "/";

// external

addJS("https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"); // google jquery CDN
addCSS(root_prefix + "css/tailwind.css");                                  // compiled tailwind stylesheet

// internal

wait_for(function(){ return window.jQuery }, function(){
  addJS(root_prefix + "js/load-templates.js");     // loads templates

  wait_for(function(){ return $('.navbar').length > 0 }, function(){
    var url = new URL(window.location.href);
    $('div.navbar div.flex a[href="'+url.pathname+'"]').each(function(){ $(this).addClass('current'); });
    console.log('highlighted current');
  });
});
