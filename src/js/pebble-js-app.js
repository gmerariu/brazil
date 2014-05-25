var forecastIOKey = "f4d0ddcd176aea8fda7a18fd9d98437d";
var stock_symbol = "IMS";
var price;
var price_change;

// make it easy to convert an icon to a number for the pebble side
var icons = [
  'clear-day',
  'clear-night',
  'cloudy',
  'fog',
  'partly-cloudy-day',
  'partly-cloudy-night',
  'rain',
  'sleet',
  'snow',
  'wind',
  'error'
];





function getAndShowWeather ( ) {
  fetchStockQuote(stock_symbol);
  navigator.geolocation.getCurrentPosition(function (position) {
    
    // position.coords.latitude, position.coords.longitude
        getCurrentWeather(position.coords.longitude, position.coords.latitude);
    });

  setTimeout(getAndShowWeather, 1800000);
}

function fetchStockQuote(symbol) {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  req.open('GET', "http://dev.markitondemand.com/Api/Quote/json?" +
    "symbol=" + symbol, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      // 200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        //var price;
      
        if (response.Data) {
          // data found, look for LastPrice
          price = response.Data.LastPrice;
          price_change = response.Data.Change;
          console.log(price);
          console.log(price_change);
        }
      }
    }
  };
  req.send(null);
}




function getCurrentWeather (lon, lat) {
  var req = new XMLHttpRequest();
    req.open('GET',"https://api.forecast.io/forecast/" + forecastIOKey + "/" + lat + "," + lon, true);
    req.onload = function(e) {
      if (req.readyState == 4 && req.status == 200) {
        if(req.status == 200) {
          var response = JSON.parse(req.responseText);

          var send = { };
          if (response.currently) {
            var icon = icons.indexOf(response.currently.icon);

            // if the icon isn't found, default to error
            if (icon === -1) {
              icon = 10;
            }

            send.icon = icon;
            send.temperature_f = Number(response.currently.temperature).toFixed(0);
            send.temperature_c = Number(FtoC(response.currently.temperature)).toFixed(0);
            send.error = Number(price_change).toFixed(2);
            send.location = Number(price).toFixed(2);
            
          }

          Pebble.sendAppMessage(send);
        } else {
          console.log("Error");
        }
      }
    };

    req.send(null);
}

function FtoC (f) {
  return (f - 32) * 5 / 9;
}

Pebble.addEventListener("ready",
  function(e) {
    setTimeout(getAndShowWeather, 2000);
   
  }
);
