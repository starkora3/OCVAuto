<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META http-equiv="content-language" content="ja">
<META http-equiv="Pragma" content="no-cache">
<title>状態モニタ</title>
<style type="text/css">
<!--
.image-upload{
max-width:<?php
	if(array_key_exists("maxw",$_REQUEST)){
		echo ($_REQUEST["maxw"] . "px;");
	}else{
		echo "400px;";
	}
?>
max-height:<?php
	if(array_key_exists("maxh",$_REQUEST)){
		echo ($_REQUEST["maxh"] . "px;");
	}else{
		echo "250px;";
	}
?>

}
.image-container{
	float:  left;
}
.image-hr{
	clear:both;
}
.text-container{
	float:  left;
	margin-left:10px;
	font-size:<?php if(array_key_exists("font",$_REQUEST)){
		echo ($_REQUEST["font"] . "px;");
	}else{
		echo "18px;";
	}?>
}
-->
</style>
</head>
<body>
<!--
<form action="upload.php" method="post" enctype="multipart/form-data">
  ファイル：<br />
  <input type="file" name="upfile" size="30" /><br />
  <br />
  <input type="submit" value="アップロード" />
</form>
-->
<?php
	require_once("./define.php");
	$mode = NULL;
	do{
		if(!array_key_exists("mode",$_REQUEST)){
			break;
		}
		$mode = $_REQUEST["mode"];
		if(strcmp($mode, MODE_VIEW) != 0){
			break;
		}

		$encSystem = "SJIS";
		if(strcasecmp(PHP_OS,"Linux")==0)
		{
			$encSystem = "UTF-8";
		}

		$dir = UPLOAD_DIR;
		if(is_dir($dir) == false)
		{
			if( mkdir($dir) == false)
			{
				echo "<font color=red><h1>フォルダの作成に失敗しました。</h1></font>";
			}
			else
			{
				chmod($dir , 0777);
			}
		}


		if ($handle = opendir($dir)) 
		{
			$arrFile = array();
			$arrDir = array();

		    while (false !== ($file = readdir($handle))) 
		    {
				if( substr($file,0,1) !== "."  
					&& $file !== "index.php" 
					&& $file !== "index.html"
					&& $file !== "define.php"
					&& $file !== "delete.php"
					&& $file !== "upload.php")
				{
					if( is_dir($file))
					{
						$arrDir[] = $file;
					}
					else
					{
						$arrFile[] = $file;
					}
					
				}
		    }
		    closedir($handle);
		    
			/*
		    sort( $arrDir , SORT_STRING );
		    foreach($arrDir as $value)
		    {
				echo("<h3><a href='" .(strlen($dir)?$dir."/":"").mb_convert_encoding($value, "UTF-8",$encSystem)."'>/".mb_convert_encoding($value, "UTF-8",$encSystem)."</a></h3>");
			}
			*/
		    sort( $arrFile , SORT_STRING );
			$disp = array();
			if( !array_key_exists("file",$_REQUEST)){
			    foreach($arrFile as $value)
			    {
					$temp = explode(".", $value);

					$msg="";
					$fMsg = $dir . "/" . $value . ".txt";
					if(is_file($fMsg)){
						$msg = file_get_contents($fMsg);
					}
					if(count($temp) == 4 ){
						$path = $dir."/".$value;
						$mtime = filemtime($path);
						$now = time();
						$old = ($now - $mtime) > 600 ? true : false;
						$date = date("Y/m/d H:i:s", $mtime);
						$param = strval(time());
						$disp[$value] ="<div class='image-container'><table>"
								."<tr><td><a href='".(strlen($dir)?$dir."/":"").mb_convert_encoding($value, "UTF-8", $encSystem)."'><img class='image-upload' src='".(strlen($dir)?$dir."/":"").mb_convert_encoding($value, "UTF-8", $encSystem)."?p=".$param."'></a></td></tr>"
								."<tr><td><div class='text-container'><a href='?mode=".$mode."&file=".mb_convert_encoding($value, "UTF-8", $encSystem)."'>"
								. mb_convert_encoding($value, "UTF-8", $encSystem)."</a>"
								. "</div><div class='text-container'> - "
								. ($old? "<font color=red size=+2><b>".$date."</b></font>":$date)
								."</div></td></tr><tr><td><textarea rows='4' cols='60' readonly>"
								.$msg
								. "</textarea></td></tr></table></div>";
					}
				}

				if(array_key_exists("sort",$_REQUEST)){
					// ソート指定があるなら
					$arrSort = explode(",", $_REQUEST["sort"]);
					foreach($arrSort as $item){
						if(strcmp($item,"hr")==0){
							echo "<hr class='image-hr'>";
							continue;
						}
						foreach($disp as $key=>$value){
							$temp = explode(".", $key);
							if(strcmp($temp[3],$item)==0){
								echo($value);
								$disp[$key]="";
								break;
							}
						}
					}
				}
				foreach($disp as $key=>$value){
					echo($value);
				}
			}else{
				$prefix = $_REQUEST["file"];
				echo "<h1>" . $prefix . "</h1><br><hr><table><tr><td>";
			    foreach($arrFile as $value)
			    {
					if( strpos($value,$prefix) ===0 &&
						( strlen($value) == strlen($prefix) ||
							 is_numeric($value[strlen($prefix)]) == FALSE) ){
						$path = $dir."/".$value;
						$mtime = filemtime($path);
						$date = date("Y/m/d H:i:s", $mtime);
						$param = strval(time());
						echo("<div class='image-container'><table>"
								."<tr><td><img class='image-upload' src='".(strlen($dir)?$dir."/":"").mb_convert_encoding($value, "UTF-8", $encSystem)."?p=".$param."'></td></tr>"
								."<tr><td align=center><table><tr><td><h3><a href='".(strlen($dir)?$dir."/":"").mb_convert_encoding($value, "UTF-8", $encSystem)."'>"
								. mb_convert_encoding($value, "UTF-8", $encSystem)."</a></td><td>" 
								. "<form type='post' action='delete.php' ><input name='fname' type='hidden' value='".rawurlencode($value)."'><input type='submit' value='削除'></form></nobr>"
								. "</h3></td><td valign=top>".$date."</td></tr></table></td>"
								. "</tr></table></div>");
					}
				}

				echo "</td></tr></table><hr><h3><a href='.?mode=".$mode."'>戻る</a></h3>";
			}
?>
<script type="text/javascript">
function doReloadNoCache() {
 
    // キャッシュを無視してサーバーからリロード
    window.location.reload(true);
 
}
// 180秒後にリロード
setTimeout(doReloadNoCache, 60000*3);
</script>
<?php		    
		}
	}while(FALSE);

	if($mode == NULL){
?>
	<form type='get' action='.'>mode : <input type="text" id="mode" name="mode" minlength="4" size="20"> <input type='submit' value='送信'></form>
<?php
	}
?> 
</body>
</html>