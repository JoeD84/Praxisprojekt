var IE6 = ( navigator.userAgent.indexOf("MSIE 6")>=0 || navigator.userAgent.indexOf("MSIE 7")>=0 ) ? true : false;
if(IE6){

	$(function(){
		
		$("<div>")
			.css({
				'position': 'absolute',
				'top': '0px',
				'left': '0px',
				backgroundColor: 'black',
				'opacity': '0.75',
				'width': '100%',
				'height': $(window).height(),
				zIndex: 5000
			})
			.appendTo("body");
			
		$("<div><img src='no-ie6.png' alt='' style='float: left;'/><p><strong>Diese Seite unterstützt Internet Explorer erst ab Version 9.</strong><br /><br />Bitte verwenden sie einen modernen Browser der CSS unterstützt! <br>Zum Beispiel: <a href='http://getfirefox.org'>Mozilla Firefox</a> oder <a href='http://www.google.de/chrome/'>Google Chrome</a>.</p>")
			.css({
				backgroundColor: 'white',
				'top': '50%',
				'left': '50%',
				marginLeft: -210,
				marginTop: -100,
				width: 410,
				paddingRight: 10,
				height: 220,
				'position': 'absolute',
				zIndex: 6000
			})
			.appendTo("body");
	});		
}