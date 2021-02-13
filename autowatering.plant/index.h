String indexPage = R"=====(
<!doctype html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport"
        content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>Autowatering Plant</title>
  <style>
    * {
      margin: 0;
      padding: 0;
    }

    a {
      text-decoration: none;
      color: #333;
    }

    body {
      font-family: 'Dosis', sans-serif;
      line-height: 1.6;
      background: rgb(180, 197, 253);
    }

    h2, h3, h4 {
      text-transform: uppercase;
      padding: .3rem;
      text-align: center;
    }


    .btn {
      background-color: #214ac0;
      color: #fff;
      display: inline-block;
      padding: .8rem 2rem;
      transition: background-color .5s;
      border: none;
      cursor: pointer;
      margin: .2rem;
    }

    .btn:hover {
      background-color: #c70a0a;
    }

    #controls {
      text-align: center;
      margin: auto;
      width: fit-content;
    }

    .control {
      border: 1px solid;
      padding: 1rem;
    }

    #logs {
      text-align: center;
      margin: 1rem auto;
      width: fit-content;
      padding: 1rem;
    }
  </style>
</head>
  <body>
    <header>
      <h3>Your autowatering system:</h3>
    </header>
    <main>
      <section id="controls">
        <div class="control">
          <h4>Control the light:</h4>
          <a class="btn" href="#" onclick="fetch('/light?turn=on').catch(console.log)">Turn ON</a>
          <a class="btn" href="#" onclick="fetch('/light?turn=off').catch(console.log)">Turn OFF</a>
        </div>
      </section>
      <section id="logs">
        <h4>Moisture sensor data:</h4>
      </section>
    </main>
    <script>
      /* Add logs */
      setInterval(() => {
        getLogMessage()
        .then(addLogMessage)
        .catch(err => console.log(err))
      }, 5000);


      const getLogMessage = () => {
        return fetch('/logs')
          .then(response => {
            if(response.ok) {
              console.log('The response is ok');
              return response.text()
            } else {
              return 'Could not reach the logs from server'
            }
          })
          .catch(err => {
            console.log(err);
            return 'Could not reach the logs from server'
          })
      }


      const addLogMessage = (msg) => {
        /* Prepare log message */
        const newMessage = document.createElement('div');
        newMessage.innerText = `${new Date().toLocaleString()}:> ${msg}`

        /* Add log message */
        const logs = document.getElementById('logs')
        const logMessages = [...logs.querySelectorAll('div')]
        logs.insertAdjacentElement('beforeend', newMessage)

        /* Remove oldest log message to make the log short */
        if(logMessages.length > 3) {
          logs.removeChild(logMessages[0])
        }
      }
    </script>
  </body>
</html>
)=====";
