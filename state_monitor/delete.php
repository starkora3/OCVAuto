<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja" lang="ja">
<head>
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META http-equiv="content-language" content="ja">
<META http-equiv="Pragma" content="no-cache">
<title>ファイル削除</title>
</head>
<body>
<font size="+2"><p><?php

$encSystem = "SJIS";
if(strcasecmp(PHP_OS,"Linux")==0)
{
	$encSystem = "UTF-8";
}
require_once("./define.php");

$dir = UPLOAD_DIR . "/";

if (isset($_REQUEST["fname"])) 
{
	$file = $dir . urldecode($_REQUEST["fname"]);
	
	$fnamedisp = mb_convert_encoding(urldecode($_REQUEST["fname"]),"UTF-8",$encSystem);
	
	if(is_file($file))
	{
		unlink($file);
		$delete = true;
		if(is_file($file))
		{
			sleep(100);
			if(is_file($file))
			{
				$delete = false;
				
			}
		}
		if($delete)
		{
			echo "<font color=blue>ファイル : <b>" . $fnamedisp ."</b> を削除しました。</font>";
		}
		else
		{
			echo "<font color=red>ファイル : <b>" . $fnamedisp ."</b> の削除に失敗しました。</font>";
		}
	}
	else
	{
		echo  " ファイルが見つかりません。";
	}
} 
else 
{
  echo "ファイルが選択されていません。";
}
?></p>

<a href="<?php echo HOME_PATH; ?>">戻る</a></font>

<br><br><pre>
<?php print_r($_REQUEST);?>
</pre>
</body>
</html>