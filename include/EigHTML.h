#include <Arduino.h>
#include <string.h>

const char StandartSeite1[] = R"rawliteral(
        <!DOCTYPE html>
        <html>

        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <meta name='author' content='Felix Girke'>
            <link rel="icon" href='https://felixesptest.000webhostapp.com/Pic/MAMAAAA.ico'>
            <script src="https://kit.fontawesome.com/7d927496f9.js" crossorigin="anonymous"></script><!--Zum logos bekommen-->
            <script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>
            <script type='text/javascript'>
                google.charts.load('current', { 'packages': ['corechart'] });
                google.charts.setOnLoadCallback(drawChart);

                function drawChart() {
                    var data = google.visualization.arrayToDataTable([
                        ['Datum', 'Liter'],
    )rawliteral";
//                        ['19.09.2020', 200],
//                        ['20.09.2020', 200],
//                        ['21.09.2020', 660],
//                        ['22.09.2020', 400],
//                        ['25.09.2020', 199]
const char StandartSeite2[] = R"rawliteral(
                    ]);

                    var options = {
                        curveType: 'none',
                        legend: 'none',
                        hAxis: {  
       							slantedText: true, 
        						slantedTextAngle: 90 // here you can even use 180 
   					     }
                    };
                    var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));

                    chart.draw(data, options);
                }
            </script>
            <style>
                html {
                    font-family: Helvetica;
                    display: inline-block;
                    margin: 0px auto;
                    text-align: center;
                }

                .button {
                    background-color: #00008B;
                    border: none;
                    color: white;
                    padding: 16px 40px;
                    text-decoration: none;
                    font-size: 30px;
                    margin: 2px;
                    cursor: pointer;
                }

                table,
                th,
                td {
                    border: 1px solid black;
                    border-collapse: collapse;
                }
            </style>
            <title>Girke's Zisterne</title>
        </head>

        <body>
            <h1>Girke's Zisterne</h1>
            <p>neue Abfrage:</p>
            <p><a href="/5/on"><button class="button">Start</button></a><a href='/'><button class="button" style='background-color:#228B22'>aktualisieren</button></a></p>
                )rawliteral";
            //<p><a href="/6/off"><button class="button" style='background-color:#3399ff'>Winterzeit aktivieren</button></a></p>
const char StandartSeite3[] = R"rawliteral(
            <div style=' width: auto; height: 500px' id='curve_chart'></div>
            <br> <br>
            <table style='width:100%'>
                <tr>
                    <th style='border-top:hidden;border-bottom:hidden;border-left-color:white'></th>
                    <th style='width:150px'>Datum</th>
                    <th style='width:70px'>Fuellh.</th>
                    <th style='width:70px'>Liter</th>
                    <th style='width:70px'>Delta</th>
                    <th style='border-top:hidden;border-bottom:hidden;border-right:hidden;'></th>
                </tr>
    )rawliteral";
//                <tr>
//                    <td style='border-top:hidden;border-bottom:hidden;border-left:hidden;'> </td>
//                    <td>19.09.2020 11:23</td>
//                    <td> 135cm</td>
//                    <td>4963l</td>
//                    <td style='border-top:hidden;border-bottom:hidden;border-right:hidden;'> </td>
//                </tr>
//                <tr>
//                    <td style='border-top:hidden;border-bottom:hidden;border-left:hidden;'> </td>
//                    <td>19.09.2020 11:23</td>
//                    <td> 135cm</td>
//                    <td>4963l</td>
//                    <td style='border-top:hidden;border-bottom:hidden;border-right:hidden;'> </td>
//                </tr>
const char StandartSeite4[] = R"rawliteral(
        </table> <br>
        <div align='left'>
            <a href='/8/on'><i class="fas fa-unlock-alt fa-3x" style="color: black;"></i></a>
            <a style='color:white' >HHH</a>
            <a href='/stat/on'><i class='fas fa-book fa-3x' style='color:#571a66'></i></a>
            <a style='color:white' >HHH</a>
            <a href='/7/on'><i class='fas fa-envelope-open-text fa-3x'style='color: green;' ;></i></a>
        </div>
        <div align='right'>
            <p align='right'>Die aktuelle Version 2.1 des Quellcodes gibt es <a href='https://github.com/Feeee23/MamasZisterne'>hier</a></p>
        </div>
        </body>

        </html>
    )rawliteral";

const char AnmeldeSeite[] = R"rawliteral(
    <!DOCTYPE HTML>
    <html>

    <head>
        <title>Anmeldedaten eingeben</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta name='author' content='Felix Girke'>
        <link rel="icon" href='https://felixesptest.000webhostapp.com/Pic/MAMAAAA.ico'>
        <style>
            html {
                font-family: Helvetica;
                display: inline-block;
                margin: 0px auto;
                text-align: center;
            }
        </style>
    </head>

    <body>
        <a href='/'><button class="button" style='background-color:#228B22'>Zurueck</button></a>
        <br> <br>
        <p>Gib bitte den Benutzername und das Passwort ein:</p>
        <br>
        <form action='/send' method="POST">
            Benutzername: <input type="text" name="Benutzername">
            <br><br>
            Passwort: <input type="password" name="Eingabe_Passwort"> <br> <br>
            <input type="submit" value="Submit">
        </form>
        <br>
    </body>
    </html>

    )rawliteral";  

const char AdminBereich[] = R"rawliteral(
    <!DOCTYPE html>
    <html>

    <head>
        <title>Admin Seite</title>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <meta name='author' content='Felix Girke'>
        <link rel="icon" href='https://felixesptest.000webhostapp.com/Pic/MAMAAAA.ico'>
        <style>
            html {
                font-family: Helvetica;
                display: inline-block;
                margin: 0px auto;
                text-align: center;
            }
        </style>

    </head>

    <body>
        <p>Gib die Abweichung des Ergebnisses ein:</p>
        <br>
        <form action="/get">
            Abweichnung: <input type="text" name="Abweichnung"> cm
            <br><br>
            <input type="submit" value="Submit">
        </form>
        <br>   
    )rawliteral";

String SendAdminBereich(String Werte, String Header){
    String s=AdminBereich;
    s+=Werte;
    s+="<p>";
    s+=Header;
    s+="</p>";
    s+="<a href='/DataDump'><button class='button' style='background-color:#7591ce'>rawDataDump</button></a> <br><br>";
    s+="<a href='/logout'><button class='button' style='background-color:#228B22'>Ausloggen</button></a> <br><br>";
    s+="<a href='/reset'><button class='button' style='background-color: red'>reset</button></a><br><br>";
    s+="<a href='/Speicherleeren'><button class='button' style='background-color: violet'>Speicher Leeren</button></a>";
    return s;
}

String SendStandartseite(String Dia, String Tab, int j){ //baut die Standart Seite zusammen
    String s=StandartSeite1;
    s+=Dia;
    s+=StandartSeite2;
    if (j==3600) { //Sommerzeit
        s+="<p><a href=\"/6/off\"><button class=\"button\" style='background-color:#3399ff'>Winterzeit aktivieren</button></a></p>";
    }else{ //winterzeit
        s+="<p><a href=\"/6/on\"><button class=\"button\" style='background-color:#ffb84d'>Sommerzeit aktivieren</button></a></p>"; 
    }
    s+=StandartSeite3;
    s+=Tab;
    s+=StandartSeite4;
    return s;
}

String SendStat(String StatValue){
    const char StatSeite[] = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>Anmeldedaten eingeben</title>
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <meta name='author' content='Felix Girke'>
            <link rel="icon" href='https://felixesptest.000webhostapp.com/Pic/MAMAAAA.ico'>
            <style>
                html {
                    font-family: Helvetica;
                    display: inline-block;
                    margin: 0px auto;
                    text-align: center;
                }
            </style>
        </head>
        <body>
            <h1>Statistik Seite</h1>
            <a href='/'><button class="button" style='background-color:#27c227'>Zurueck</button></a>
            <br><br>
        )rawliteral";
    String s= StatSeite;
    s+=StatValue;
    return s;
}
