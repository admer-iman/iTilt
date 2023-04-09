let reqURL = `${window.location.href}`;
let dataField = document.querySelectorAll(".data") ; 
console.log(dataField);
let interval = 2000;


// send a request to the server to send back the value sended by ESP
setInterval(() => {
  // start fitch 
  fetch(reqURL, {
    method: "POST",
    credentials: "include",
    body: JSON.stringify({}),
    cache: "no-cache",
    headers: new Headers({
      "content-type": "application/json"
    }),
  }).then((response) => {
  // read the data send by the server in show in the page dynamicly  
    response.json().then((data) => {
      console.log(data);
      dataField[0].innerHTML = data.tilt ;
      dataField[1].innerHTML = data.roll ;
      dataField[2].innerHTML = data.grav ;
      dataField[3].innerHTML = data.abv ;
      dataField[4].innerHTML = data.tem ;
      dataField[5].innerHTML = data.intemp ;
    })

  }) // End Reponse 
  // end fitch

}, interval);