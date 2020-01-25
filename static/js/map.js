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
    let rawdata, data = [], gradient;
    let selectedValue = $('input[name="options"]:checked').val();
    let periode = $('#periode-selector option:selected').val();
    console.log(selectedValue);
    rawdata = getData(selectedValue, periode);
    gradient = getGradient(selectedValue);

    if (selectedValue === "air"){

    }else{
        json = JSON.parse(rawdata);
        console.log(json);
        for (var line in json){
            var dot = {};
            dot['location'] = new google.maps.LatLng(line['lattitude'], line['longitude']);
            dot['weight'] = line['value'];
            data.push(dot);
        }
    }

    console.log(data);

    //data = getTemperatureData();
    heatmap.setData(data);
    heatmap.set('gradient', gradient);
    heatmap.setMap(map);
}

function getData(capteur, periode){
    let timestamp = new Date().getTime();
    let url = "http://127.0.0.1:5000/get?timestamp=" + timestamp + "&delta=" + 0 + "&type=" + capteur;
    console.log(url)
    let xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", url, false);
    xmlHttp.send();
    console.log(xmlHttp.responseText);

    return xmlHttp.responseText;
}

function getGradient(capteur){
    let gradient;
    switch(capteur){
        case 'son':
            gradient = getNoiseGradient();
            break;
        case 'temperature':
            gradient = getTemperatureGradient();
            break;
        case 'air':
            gradient = getAirGradient();
            break;
        case 'humidity':
            gradient = getHumidityGradient();
            break;
        case 'waste':
            gradient = getWasteGradient();
            break;
    }
    return gradient;
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
    let gradient = [
        'rgba(237,237,237, 0)',
        'rgba(219,219,219, 1)',
        'rgba(201,201,201, 1)',
        'rgba(183,183,183, 1)',
        'rgba(165,165,165, 1)',
        'rgba(146,146,146, 1)',
        'rgba(128,128,128, 1)',
        'rgba(110,110,110, 1)',
        'rgba(92,92,92, 1)',
        'rgba(74,74,74, 1)',
        'rgba(56,56,56, 1)',
        'rgba(38,38,38, 1)'
    ];
    return gradient;
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