// Listen for when the watchface is opened.
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS is ready for requests!");
  }
);

// Listen for when an AppMessage is received.
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage has been received!");
    console.log(JSON.stringify(e.payload));
    
    // What is the key in the AppMessage?
    if ("KEY_PAIRING_DIGIT1" in e.payload) {
      
    } else if ("KEY_NEXT" in e.payload) {
      console.log("Next slide");
    } else if ("KEY_PREV" in e.payload) {
      console.log("Previous slide");
    }
  }                     
);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
