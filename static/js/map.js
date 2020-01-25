var map, heatmap;
var tempHeatmap, noiseHeatmap, humidityHeatmap, wasteHeatmap;

function initMap() {
    let ensea = {lat: 49.039319, lng: 2.071970};

    map = new google.maps.Map(
        document.getElementById('map'), {
            zoom: 15, 
            center: ensea, 
            disableDefaultUI: true,
            mapTypeId: 'roadmap',
            mapTypeControl: true,
            mapTypeControlOptions: {
                style: google.maps.MapTypeControlStyle.HORIZONTAL_BAR,
                position: google.maps.ControlPosition.BOTTOM_CENTER
          },
    });

    tempHeatmap = new google.maps.visualization.HeatmapLayer();
    noiseHeatmap = new google.maps.visualization.HeatmapLayer();
    humidityHeatmap = new google.maps.visualization.HeatmapLayer();
    wasteHeatmap = new google.maps.visualization.HeatmapLayer();

    //HeatMap test
    heatmap = new google.maps.visualization.HeatmapLayer();
    heatmap.setData(getTemperatureData());
    displayHeatmap(heatmap);
      
}

function displayHeatmap(heatmap){
    heatmap.setMap(map)
}

function refresh(){
    let data, gradient;
    let selectedValue = $('input[name="options"]:checked').val();
    let periode = $('#periode-selector option:selected').val();
    console.log(selectedValue);
    getData();

    switch(selectedValue){
        case 'son':
            data = getNoiseData();
            gradient = getNoiseGradient();
            break;
        case 'temperature':
            data = getTemperatureData();
            gradient = getTemperatureGradient();
            break;
        case 'air':
            data = getAirData();
            gradient = getAirGradient();
            break;
        case 'humidity':
            data = getHumidityData();
            gradient = getHumidityGradient();
            break;
        case 'waste':
            data = getWasteData();
            gradient = getWasteGradient();
            break;
    }

    heatmap.setData(data);
    heatmap.set('gradient', gradient);
    heatmap.setMap(map);
}

function getData(){
    let xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", "https://enseack-rest-sql.herokuapp.com/all", false);
    xmlHttp.send();
    console.log(xmlHttp.responseText);
}

//TODO: Recuperer données temperature
function getTemperatureData(){
    heatmapData = [
        new google.maps.LatLng(49.0393, 2.0714),
        new google.maps.LatLng(49.0392, 2.0710),
        new google.maps.LatLng(49.0392, 2.0717),
        new google.maps.LatLng(49.0394, 2.0713),
        new google.maps.LatLng(49.0399, 2.0713),
        new google.maps.LatLng(49.0395, 2.0714),
        new google.maps.LatLng(49.0394, 2.0712),
        new google.maps.LatLng(49.0394, 2.0711),
        new google.maps.LatLng(49.0390, 2.0715),
        new google.maps.LatLng(49.0397, 2.0719),
        new google.maps.LatLng(49.0396, 2.0718),
        new google.maps.LatLng(49.0390, 2.0710),
        new google.maps.LatLng(49.0392, 2.0711),
        new google.maps.LatLng(49.0391, 2.0714),
    ];

    return heatmapData
}

//TODO: Recuperer données bruit
function getNoiseData(){
    return getTemperatureData();
}

//TODO: Recuperer données déchets
function getWasteData(){
    return getTemperatureData();
}

//TODO: Recuperer données humidité
function getHumidityData(){
    return getTemperatureData();
}

//TODO: Recuperer données air
function getAirData(){
    return getTemperatureData();
}

function getNoiseGradient(){

}

function getTemperatureGradient(){

}

function getHumidityGradient(){
    let gradient = [
        'rgba(234,242,251, 0)',
        'rgba(212,230,247, 1)',
        'rgba(191,217,242, 1)',
        'rgba(170,204,238, 1)',
        'rgba(149,191,234, 1)',
        'rgba(128,179,230, 1)',
        'rgba(106,166,225, 1)',
        'rgba(85,153,221, 1)',
        'rgba(64,140,217, 1)',
        'rgba(43,128,212, 1)',
        'rgba(21,115,208, 1)',
        'rgba(0,102,204, 1)'
    ];
    return gradient;
}

function getAirGradient(){

}

function getWasteGradient(){
    let gradient = [
        'rgba(242, 238, 234, 0)',
        'rgba(230,221,212, 1)',
        'rgba(217,204,191, 1)',
        'rgba(204,187,170, 1)',
        'rgba(191,170,149, 1)',
        'rgba(179,153,128, 1)',
        'rgba(166,136,106, 1)',
        'rgba(153,119,85, 1)',
        'rgba(140,102,64, 1)',
        'rgba(128,85,43, 1)',
        'rgba(115,68,21, 1)',
        'rgba(102,51,0, 1)'
    ];
    return gradient;
}