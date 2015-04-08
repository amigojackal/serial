<?php
function tick(){
	$tick = "x".date("ymdHis",time())."z";
	echo system("echo {$tick} > /dev/ttyAMA0");
	echo "echo {$tick} > /dev/ttyAMA0\n";
}

echo system("ntptime");
for($i=0;$i<50;$i++){
	tick();
	sleep(1);
}
?>
