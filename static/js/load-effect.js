console.log("henlo");

effect = function(){
  // adding transition css property using js because of complications
  // on page load when added directly in stylesheet  
  var transition_style = $('<style>.section-content { transition: 1s; }</style>');
  $('html > head').append(transition_style);
  
  var sections = $(".section");
  var i, num_done, j;//, num_hidden = 0;
  var animation_lenght = 1000;
  var fade_in_delta = 0.5 * animation_lenght;
  var min_scroll_height = [];
  var rect;

  console.log(sections);
  
  function getMinScrollHeight( i ){
    rect = sections[i].getBoundingClientRect();
    return (rect.top * 0.9 + rect.bottom * 0.1) - window.innerHeight;
  }
  
  i = num_done = j = 0;
  //console.log();
  while( i < sections.length && getMinScrollHeight(i) <= window.scrollY ){
    setTimeout(function(){
      sections[j].children[0].className = sections[j].children[0].className + " visible-content";
      j++;
      setTimeout(function(){ num_done++ }, animation_lenght);
    }, fade_in_delta * (i - num_done));
    i++;
  }
  
  $(window).scroll(function(){// on scroll
    var max_scroll = sections[sections.length - 1].getBoundingClientRect().bottom - window.innerHeight;
    if( window.scrollY > max_scroll )
      window.scrollY = max_scroll;
    while( i < sections.length && getMinScrollHeight(i) <= window.scrollY ){
      setTimeout(function(){
	sections[j].children[0].className = sections[j].children[0].className + " visible-content";
	j++;
	setTimeout(function(){ num_done++ }, animation_lenght);
      }, fade_in_delta * (i - num_done));
      i++;
    }
  });
};

if( document.readyState == "complete" )
  effect();
else
  window.onload = effect;
