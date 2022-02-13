<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META http-equiv="content-language" content="ja">
<META http-equiv="Pragma" content="no-cache">
<title>アップロード</title>
</head>
<body>
<font size="+2"><p><?php

$BACKUP_COUNT=10;

$encSystem = "SJIS";
if(strcasecmp(PHP_OS,"Linux")==0)
{
	$encSystem = "UTF-8";
}

require_once("./define.php");

$fnamedisp = @$_FILES["upfile"]["name"];
$fname = mb_convert_encoding(@$_FILES["upfile"]["name"],$encSystem,"UTF-8");

$dest = UPLOAD_DIR;
if( !is_dir($dest) ){ 
	mkdir($dest);
}
$dest .="/";

$temp = explode(".", $fname);

if(count($temp) == 4 &&
	intval($temp[0]) == 192 && 
	intval($temp[1]) == 168){

	// バックアップ生成
	if(is_file($dest . $fname)){
		for($idx = $BACKUP_COUNT-1;$idx>0;$idx--){
			$cp_dest = $fname . "_" . sprintf('%02d',strval($idx+1)) . ".jpg";
			$cp_src = $fname . "_" . sprintf('%02d',strval($idx)) . ".jpg";

			if(is_file($dest.$cp_dest)){
				unlink($dest.$cp_dest);
			}
			if(is_file($dest.$cp_src)){
				@rename($dest.$cp_src,$dest.$cp_dest);
			}
		}

		$cp_dest = $fname . "_01.jpg";
		if(is_file($dest.$cp_dest)){
			unlink($dest.$cp_dest);
		}
		@rename($dest.$fname,$dest.$cp_dest);
	}
	
	if (is_uploaded_file($_FILES["upfile"]["tmp_name"])) {
	  if (move_uploaded_file($_FILES["upfile"]["tmp_name"], $dest .$fname)) 
	  {
	    chmod($dest . $fname, 0777);
	    echo $fnamedisp. "をアップロードしました。";

		$fMsg = $dest . $fname . ".txt";
		$msg = date('Y/m/d H:i:s ') . $fname. " をアップロードしました。";
		if(array_key_exists("msg",$_REQUEST)){
			$temp = $_REQUEST["msg"];
			$msg = date('Y/m/d H:i:s ') . $temp;
		}
		$temp="";
		if(is_file($fMsg)){
			$temp = file_get_contents($fMsg);
			$line = explode("\n",$temp);
			if( count($line) > 100){
				$line = array_slice($line, 0, 100); 
			}
			$temp = implode("\n", $line);
		}
		$msg = $msg . "\r\n" . $temp;
		file_put_contents($fMsg,$msg);
	  } else {
	    echo "ファイルをアップロードできません。";
	  }
	} else {
	  echo "ファイルがありません。";
	}
}else{echo "ファイルをアップロードできません。";
}
?></p>

<a href="<?php echo HOME_PATH; ?>">戻る</a></font>

<br><br><pre>
<?php print_r($_FILES);?>
</pre>
</body>
</html>