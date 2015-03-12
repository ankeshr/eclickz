<?php

		/*
				________________________________________________________________________________

						  PROGRAM : ASP : Admin : Main Options Admin Center (PHP)
        VERSION : 2.11
        Purpose :
      ________________________________________________________________________________
         Copyright and Legal Notices:

           All source code, images, programs, files included in this distribution
           Copyright (c) eClickZ Interactive Services LLC.  All Rights Reserved.

           Use, distibution, sale, or access to this program is forbidden without
           permission, except were permitted by the license agreement.
      ________________________________________________________________________________
         Special Instructions and notes...
      ________________________________________________________________________________
   */


		global $PROGRAMS;
		$PROGRAMS[ 'MAIN' ] = "designer";

		function config_MAIN ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $page_templates;
				global $menuitems;
				$vars[ 'Administrative' ][ '001' ]                    = array ( "Program Licensing" , "HEADER" , "" );
				$vars[ 'Administrative' ][ '002:license_number' ]     = array ( "Your License Number" , "TEXT" , "" );
				$vars[ 'Administrative' ][ '003:license_code' ]       = array ( "Your License Code/Key" , "TEXTAREA" , "" );
				$vars[ 'Administrative' ][ '100' ]                    = array ( "Access Control" , "HEADER" , "" );
				$vars[ 'Administrative' ][ '101:admin_pw' ]           = array ( "<b>Admin Password</b><br />Login to admin center with the 'admin' username and this password to gain full control of the system." , "TEXT" , "admin" );
				$vars[ 'Administrative' ][ '102:designer_pw' ]        = array ( "<b>Designer Password</b><br />Login to admin center with the 'designer' username and this password to gain control over just the design/visual elements of the system." , "TEXT" , "admin" );
				$vars[ 'Administrative' ][ '103:content_pw' ]         = array ( "<b>Content Manager Password</b><br />Login to admin center with the 'contentmanager' username and this password to gain control over just the category/listings/keywords areas of the system." , "TEXT" , "admin" );
				$vars[ 'Administrative' ][ '104:affmanager_pw' ]      = array ( "<b>Affiliate Manager Password</b><br />Login to admin center with the 'affiliatemanager' username and this password to gain control over just the affiliate related parts of the system." , "TEXT" , "admin" );
				$vars[ 'Administrative' ][ '200' ]                    = array ( "Basic Settings" , "HEADER" , "" );
				$vars[ 'Administrative' ][ '201:email' ]              = array ( "Default Email Address" , "TEXT" , "webmaster\@yoursite.com" );
				$vars[ 'Administrative' ][ '202:admin_title' ]        = array ( "Admin Center Title" , "TEXT" , "iWeb Software Administration Center" , "design" );
				$vars[ 'Administrative' ][ '203:gmt_offset' ]         = array ( "How many hours away from GMT are you" , "SELECT" , "-12|-11|-10|-9|-8|-7|-6|-5|-4|-3|-2|-1|0|+1|+2|+3|+4|+5|+6|+7|+8|+9|+10|+11|+12" );
				$vars[ 'Administrative' ][ '204:php_cache' ]          = array ( "PHP Settings Cache?<br /><b>Note</b>: The 'memcached' option requires installation and setup of the memcached server daemon, as well as the memcache PECL Library.  Please refer to the Hyperseek Documentation/Knowledge Base for more information on this option.  Do not use the 'memcached' option until/unless you have these items installed on your server!" , "SELECT" , "NO|Standard|memcached|apc" );
				$vars[ 'Administrative' ][ '205:memc_servers' ]       = array ( "List of memcached servers. For example '127.0.0.1','192.168.133.66'." , "TEXT" , "127.0.0.1" );
				$vars[ 'Administrative' ][ '206:memc_port' ]          = array ( "Memcached server port" , "TEXT" , "11211" );
				$vars[ 'Administrative' ][ '207:config_sync_script' ] = array ( "Script used to sync config to slaves. Relative to homedir, ex: iweb-new/scripts/conf_sync.sh" , "TEXT" , "iweb-new/scripts/conf_sync.php" );
				// $vars['MAIN']['24:editor_preference'] = array("Which Template Editor Mode do you prefer","SELECT","STANDARD|WYSIWYG (MSIE 4+ only)", "design");
				// $vars['MAIN']['25:admin_nav_bar'] = array("Which Admin Navigation 'Skin' Do you prefer","SELECT","STANDARD|Outlook Style");
				$vars[ 'URLS' ][ '10' ]                   = array ( "URLs and Directories" , "HEADER" , "" );
				$vars[ 'URLS' ][ '11:html_dir' ]          = array ( "URL of Help Documents" , "TEXT" , "http://platform.eclickz.com/help" );
				$vars[ 'URLS' ][ '12:button_dir' ]        = array ( "URL to images directory" , "TEXT" , "./images" );
				$vars[ 'URLS' ][ '13:script_dir' ]        = array ( "URL to include (scripts/css) directory" , "TEXT" , "./include" );
				$vars[ 'Style' ][ '01' ]                  = array ( "Colors and Styles" , "HEADER" , "" );
				$vars[ 'Style' ][ '02:pg_color' ]         = array ( "Default Page Color" , "TEXT" , "white" , "design" );
				$vars[ 'Style' ][ '03:win_color' ]        = array ( "Window Color" , "TEXT" , "#e0e0e0" , "design" );
				$vars[ 'Style' ][ '04:title_color' ]      = array ( "Title Bar Color" , "TEXT" , "navy" , "design" );
				$vars[ 'Style' ][ '05:ttxt_color' ]       = array ( "Title Bar Text Color" , "TEXT" , "white" , "design" );
				$vars[ 'Style' ][ '06:text_color' ]       = array ( "Regular Text Color" , "TEXT" , "black" , "design" );
				$vars[ 'Style' ][ '07:font_face' ]        = array ( "Default FONT Face" , "TEXT" , "Arial" , "design" );
				$vars[ 'Style' ][ '08:font_size' ]        = array ( "Default FONT Size" , "TEXT" , "-1" , "design" );
				$vars[ 'Style' ][ '09:default_language' ] = array ( "Default Language" , "SELECT" , "GENERATE:get_language_options" , "design" );
				$vars[ 'Style' ][ '09:optimize_html' ]    = array ( "Optimize (shrink) HTML Output?" , "SELECT" , "NO|YES" , "design" );
				$page_templates[ pages ]                  = "default|ok|error|accounts|account-login|account-create";
		}

?>
