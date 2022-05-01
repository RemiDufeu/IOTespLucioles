//
// Cote UI de l'application "lucioles"
//
// Auteur : G.MENEZ
// RMQ : Manipulation naive (debutant) de Javascript
//
const node_url = 'https://dashboard.heroku.com/'

async function init() {
    topBar()

    const espdata = await fetch(node_url+'M1Miage2022').then(res => res.json()).then(res => res.filtred)

    
    var map = L.map('map').setView([20, 0], 3);
    L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}', {
    attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors, Imagery © <a href="https://www.mapbox.com/">Mapbox</a>',
    maxZoom: 18,
    id: 'mapbox/dark-v10',
    tileSize: 512,
    zoomOffset: -1,
    accessToken: 'sk.eyJ1IjoicmVtb2VlZmVmIiwiYSI6ImNsMm5xZXlkczBzc3ozZW1hMGVvcDY5MzEifQ.sAR3eB11nI2eJesr5QIxTA'
}).addTo(map);

    console.log(espdata)
    espdata.forEach(esp => {
        L.marker([esp.loc.lat,esp.loc.lgn]).bindPopup("Temperature : "+ esp.temperature).addTo(map);
    })

};

window.onload = init;


