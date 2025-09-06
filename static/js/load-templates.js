// yep, i biult my own template engine

$('head').append('<div id="template-list"></div>');         // make a container for all the templates
$('#template-list').load("/templates/templates.html");      // import templates

wait_for(function(){ return $('template').length > 0 }, function(){ // wait for templates to load
  $('template').each(function() {                           // cicle through them
    var temp_content = this.innerHTML;
    var num_instances = 0;

    $("[temp-import='" + this.id + "']").each(function(){   // cicle through templates we have to replace
      var current_content = temp_content;
      var remaining_attr = ""; // atributes not specified in template

      num_instances++;
      
      $.each(this.attributes, function() { // go through atributes
        if( !this.specified || this.name == 'temp-import' )
          return;

        if( this.name.search("temp-") < 0 ){
          remaining_attr += ` ${this.name}="${this.value}"`;
          return;
        }

        // if this atribute is a placeholder replace it in the template
        current_content = current_content.replace(
          new RegExp(`__${this.name.replace('temp-', '')}__`, 'g'),
          this.value
        );
      });
      
      // the content of the element should not be a atribute
      // because this way we can make nested templates 
      current_content = current_content.replace('__content__', this.innerHTML);

      // any atributes that are not placeholders will be inherited
      current_content = current_content.replace('__attrib__', remaining_attr);
      
      this.outerHTML = current_content;
    });

    console.log("template: " + this.id + ", replaced " + num_instances + " instances");
  });
}, 100);
