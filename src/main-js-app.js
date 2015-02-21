var xhrRequest = function(url, type, callback, params) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  if (type == "POST") {      
    // Send the proper header information along with the request.
    xhr.setRequestHeader("Content-type", "application/json");
    xhr.send(JSON.stringify(params));
  }
  else { xhr.send(); }
};

function sendPairingRequest(digit1, digit2, digit3, digit4) {
  var url = "http://pptraas.herokuapp.com/raas/validate/";
  console.log("Sending pairing request...");
  xhrRequest(url, "POST",
    function(responseText) {
      console.log("Pairing request received output: " + responseText);
      try {
        // responseText contains a JSON object.
        var response = JSON.parse(responseText);
        return response.channel !== "0000";
      } catch (e) {
        console.log("Output is incorrectly formatted.");
        return false;
      }
    },
    {
      "code1": digit1,
      "code2": digit2,
      "code3": digit3,
      "code4": digit4
    }
  );
}

function sendNextSlideRequest() {
  var url = "http://pptraas.herokuapp.com/raas/next/";
  console.log("Sending request for the next slide...");
  xhrRequest(url, "GET",
    function(responseText) {
      console.log("Next slide request received output: " + responseText);
      try {
        // responseText contains a JSON object.
        var response = JSON.parse(responseText);
        return response.channel !== "0000";
      } catch (e) {
        console.log("Output is incorrectly formatted.");
        return false;
      }
    },
    { }
  );
}

function sendPrevSlideRequest() {
  var url = "http://pptraas.herokuapp.com/raas/previous/";
  console.log("Sending request for the previous slide...");
  xhrRequest(url, "GET",
    function(responseText) {
      console.log("Previous slide request received output: " + responseText);
      try {
        // responseText contains a JSON object.
        var response = JSON.parse(responseText);
        return response.channel !== "0000";
      } catch (e) {
        console.log("Output is incorrectly formatted.");
        return false;
      }
    },
    { }
  );
}

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
    if ("KEY_PAIRING_CODE_DIGIT1" in e.payload) {
      var digit1 = e.payload.KEY_PAIRING_CODE_DIGIT1;
      var digit2 = e.payload.KEY_PAIRING_CODE_DIGIT2;
      var digit3 = e.payload.KEY_PAIRING_CODE_DIGIT3;
      var digit4 = e.payload.KEY_PAIRING_CODE_DIGIT4;
      var pairingRequestResult = sendPairingRequest(digit1, digit2, digit3, digit4);
      if (pairingRequestResult === true) {
        console.log("Pairing was successful");
      } else {
        console.log("Pairing was unsuccessful.");
      }
    } else if ("KEY_NEXT" in e.payload) {
      var nextSlideRequestResult = sendNextSlideRequest();
      if (nextSlideRequestResult) {
        console.log("Request for next slide was successful");
      } else {
        console.log("Request for next slide was unsuccessful.");
      }
    } else if ("KEY_PREV" in e.payload) {
      var prevSlideRequestResult = sendPrevSlideRequest();
      if (prevSlideRequestResult) {
        console.log("Request for previous slide was successful");
      } else {
        console.log("Request for previous slide was unsuccessful.");
      }
    }
  }                     
);
