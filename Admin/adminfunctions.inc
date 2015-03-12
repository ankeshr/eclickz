<?php

		 /*
			 	________________________________________________________________________________

					  	PROGRAM : ASP : Admin : Main Admin Center (PHP)
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


		################################################################################
		# Validate the user
		################################################################################
		/** @noinspection PhpInconsistentReturnPointsInspection */
		function validate ()
		{
				global $config;
				global $PHP_AUTH_USER;
				global $PHP_AUTH_PW;
				global $REMOTE_USER;
				global $ADMIN_TYPE;
				global $CGI;
				global $COOKIES;
				$admin_user         = 'admin';
				$admin_password     = $config[ 'admin_pw' ];
				$design_user        = 'designer';
				$design_password    = $config[ 'designer_pw' ];
				$content_user       = 'contentmanager';
				$content_password   = $config[ 'content_pw' ];
				$affiliate_user     = 'affiliatemanager';
				$affiliate_password = $config[ 'affmanager_pw' ];
				// Possibly got a login form login ?
				if ( $CGI[ 'adminuser' ] && $CGI[ 'adminpassword' ] ) {
						$PHP_AUTH_USER = $CGI[ 'adminuser' ];
						$PHP_AUTH_PW   = $CGI[ 'adminpassword' ];
				} elseif ( $COOKIES[ 'adminuser' ] && $COOKIES[ 'adminpassword' ] ) {
						$PHP_AUTH_USER = $COOKIES[ 'adminuser' ];
						$PHP_AUTH_PW   = $COOKIES[ 'adminpassword' ];
				}
				// maybe we have caught authentication data in $_SERVER['REMOTE_USER']
				if ( preg_match ( '/Basic\s+(.*)$/i' , $REMOTE_USER , $matches ) ) {
						list( $uname , $pwd ) = explode ( ':' , base64_decode ( $matches[ 1 ] ) );
						$PHP_AUTH_USER = strip_tags ( $uname );
						$PHP_AUTH_PW   = strip_tags ( $pwd );
				}
				if ( !isset( $PHP_AUTH_USER ) ) {
						header ( 'WWW-Authenticate: Basic realm="Valid Username/Password Required"' );
						header ( 'HTTP/1.0 401 Unauthorized' );
						//Result if user hits cancel button
						login_error ();
				} else {
						//check the login and password
						if ( $PHP_AUTH_USER == $admin_user && $PHP_AUTH_PW == $admin_password ) {
								$ADMIN_TYPE = "administrator";

								return 1;
						} else if ( $PHP_AUTH_USER == $design_user && $PHP_AUTH_PW == $design_password ) {
								$ADMIN_TYPE = "designer";

								return 1;
						} else if ( $PHP_AUTH_USER == $content_user && $PHP_AUTH_PW == $content_password ) {
								$ADMIN_TYPE = "content";

								return 1;
						} else if ( $PHP_AUTH_USER == $affiliate_user && $PHP_AUTH_PW == $affiliate_password ) {
								$ADMIN_TYPE = "affiliate";

								return 1;
						} else {
								//This re-asks three times the login and password.
								header ( 'WWW-Authenticate: Basic realm="Valid Username/Password Required"' );
								header ( 'HTTP/1.0 401 Unauthorized' );
								//Result if user does not give good login and pass
								login_error ();
						}
				}
		}

		function login_error ()
		{
				global $language;
				global $config;
				global $ENV;
				global $SCRIPT_NAME;
				$dump = var_export ( $ENV , 1 );
				echo "
           <html>
           <head>
               <title>$config[admin_title]</title>
               <style>
                    .boxy {
                         border-right: black 3px inset;
                         padding-right: 10px;
                         border-top: silver 1px ridge;
                         padding-left: 10px;
                         z-index: 1;
                         background: white;
                         padding-bottom: 10px;
                         border-left: silver 1px ridge;
                         width: 500px;
                         padding-top: 10px;
                         border-bottom: black 3px inset;
                         font-family: verdana, arial, helvetica, sans-serif;
                         font-size: 11px;
                         align: center;
                    }
               </style>
           </head>
           <body bgcolor='white' text='#000000' link='#0000ff' vlink='#0000ff'>
           <br><br>
           <form action='$SCRIPT_NAME' method='post'>
           <table width=500 border=0 cellspacing=0 cellpadding=0 bgcolor='white' align='center'>
               <tr>
                   <td>
                     <div class='boxy'>

                        <table width=500>
                          <tr>
                             <td nowrap>
                               <img src='$config[button_dir]/key.gif'>
                             </td>
                             <td style='font-family:verdana,arial; font-size: 11px; color:navy' nowrap>
                               <b>&#160;$language[login_form_title]</b>
                               <br><br>

                               User ID:
                               <input name='adminuser' />
                               <br />

                               Password:
                               <input name='adminpassword' type='password' />
                               <br />
                               <center>
                                   <input type='submit' value='Login' />
                               </center>

                             </td>
                           </tr>
                           <tr>
                              <td style='font-family:verdana,arial; font-size: 11px; color:black' colspan=2 align='left' nowrap>
                                 <b>eClickZ Interactive Services, LLC</b> &copy; 2003 - 2010
                                 <a style='font-family:verdana,arial; font-size: 11px; color:black' href='{$config['main_url']}' target='new'>eClickZ</a>
                              </td>
                              <td style='font-family:verdana,arial; font-size: 11px; color:black' colspan=2 align='right' nowrap>
                                 <a style='font-family:verdana,arial; font-size: 11px; color:black' href='mailto:technical@eclickz.com' target='new'>Tech Support</a>
                              </td>
                           </tr>
                        </table>

                     </div>
                   </td>
                 </tr>
           </table>
           </form>
           </body>
           </html>
       ";
				exit;
		}

		// --------------------------------------------------------------------- //
		//  Admin Output
		// --------------------------------------------------------------------- //
		function admin_output ( $title = '' , $content = '' )
		{
				global $config;
				global $CGI;
				$title     = preg_replace ( "/\'/" , "\\\'/" , $title );
				$title     = preg_replace ( "/\r|\n|\cM/" , "" , $title );
				$helpadmin = $CGI[ 'admin' ];
				if ( !$helpadmin ) {
						$helpadmin = "admin";
				}
				$bg = "background-image:url('$config[button_dir]/txt-bac.gif')";
				echo "
          <html>
          <head>
            <title>iWeb Admin: $title</title>
             <style>
                table        { font-family: Arial; font-size: 10pt }
                td           { font-family: Arial; font-size: 10pt }
                tr           { font-family: Arial; font-size: 10pt }
                A:hover      { text-decoration: none; color:black; }

                input, textarea {
                 font-family: Verdana, Arial, serif, sans-serif;
                 border : thin groove;
                 background-color : #d9d9d9;
                 color:navy;
                 font-weight : normal;
                 font-size: 10px;
                }

                select {
                 font-family: Verdana, Arial, serif, sans-serif;
                 background-color : #d9d9d9;
                 color:navy;
                 font-weight : normal;
                 font-size: 10px;
                }

             </style>

             <script language='javascript'>

                   if ( top.frames['blueframe'] ) {
                        var NS4 = (document.layers) ? 1 : 0;
                        var IE = (document.all) ? 1 : 0;
                        NS = (navigator.appName=='Netscape' && navigator.plugins['LiveAudio'])? 1:0;
                        var DOM = 0;
                        if (parseInt(navigator.appVersion) >=5) {DOM=1};
                        var ver4 = IE||NS? 1:0;

                        var content = '<body style=\"margin:0px; padding:0px; border:1px solid silver;background:silver\"> <table width=100% hspace=0 vspace=0><tr> <td valign=\"top\" align=\"left\" style=\"font-family:verdana,arial,helvetica; font-size:9pt; font-weight:bold;color:navy\">$title</td><td valign=\"top\" align=\"right\"><a href=\"javascript:top.openHelp(\'$helpadmin\',\'$CGI[action]\');\"> <img src=\"$config[button_dir]/help-icon.gif\" border=0></a></td></tr></table></body>';


                        if (DOM) {
                           var cdiv = top.blueframe.document.getElementById('bluecontent');
                        }
                        else if (IE) {
                           var cdiv = top.blueframe.document.all['bluecontent'];
                        }
                        cdiv.innerHTML = content;
                   }
                   else {
                       top.document.title = '$title';
                   }
             </script>
          </head>
          <body bgcolor='#efefef' link='navy' vlink='navy' leftmargin='10' topmargin='10'>
            <font face='arial,helvetica' size=-1>
                   $content
            </font>
           </body>
           </html>
        ";
				exit;
		}

?>
