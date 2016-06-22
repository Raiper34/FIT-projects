$(document).ready(function() {
	//alert("Document is ready");
	$('#rollover-box h2').click(function() {
		if($("#rollover-box div").css("height") != '0px')
		{
			$("#rollover-box div").animate({height: '0'}, function() {$("#rollover-box div").hide();});
			localStorage.box = 0;
		}
		else
		{
			$("#rollover-box div").animate({height: '150'});
			$("#rollover-box div").show();
			localStorage.box = 150;
		}
	});

	$("#rollover-box div").css("height", ((localStorage.box == null)? '150' : localStorage.box));
	if(localStorage.box == 0)
	{
		$("#rollover-box div").hide();
	}
})