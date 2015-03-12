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


		class Admin
		{

				var $output;

				function Admin ()
				{
						$this->output = '';
				}


				################################################################################
				# Run the requested action
				################################################################################
				function go ()
				{
						global $CGI;
						$action = $CGI[ 'action' ];
						if ( !$action ) {
								$action = "draw_frames";
						}
						if ( $action == "Import Language Variables" ) {
								$action = "import_language";
						}
						if ( method_exists ( $this , $action ) ) {
								call_user_func ( array ( &$this , $action ) );
						} elseif ( function_exists ( $action ) ) {
								call_user_func ( $action );
						} else {
								admin_output ( "ERROR" , "Invalid Action: $action" );
								exit;
						}
						if ( preg_match ( "/frame/" , $action ) ) {
								echo $this->output;
						}
				}


				################################################################################
				# Main Admin Frames
				################################################################################
				function draw_frames ()
				{
						global $config;
						global $ADMIN_TYPE;
						if ( $ADMIN_TYPE != 'administrator' ) {
								$this->draw_frames_simple ();
						} else {
								$this->draw_frames_full ();
						}
						exit;
				}

				function draw_frames_full ()
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $CGI;
						global $ADMIN_TYPE;
						echo <<<END

           <html>
              <head>
                  <title>$config[admin_title]</title>

                  <script language='javascript'>

                    var MSIE = (document.all) ? 1 : 0;

                    function openHelp( admin, action ) {
                        var Loc = '$config[html_dir]' + '/help.php?admin=' + admin + '&action=' + action;
                        help=open('', 'help','fullscreen=yes');
                        help.moveTo(0,0);
                        help.resizeTo(screen.width,screen.height);
                        help.document.location.href=Loc;
                        help.focus();
                    }

                    function draw_bottom_frame() {

                         var height = 640;

                         if (parseInt(navigator.appVersion)>3) {
                             height = window.innerHeight;
                             if ( MSIE ) { height = parent.document.body.offsetHeight; }
                         }

                         height = height - 70;

                         document.write("<iframe scrolling='auto' name='main' width='100%' height='" + height + "' src='$SCRIPT_NAME?action=main_frame'></iframe>");
                    }

                    function draw_utilities_frame() {

                         document.write("<iframe style='overflow:hidden' name='menu' width='245' height='200' src='$SCRIPT_NAME?action=config_list'></iframe>");
                    }

                    function draw_app_frame() {

                         var height = 640;

                         if (parseInt(navigator.appVersion)>3) {
                             height = window.innerHeight;
                             if ( MSIE ) { height = parent.document.body.offsetHeight; }
                         }

                         height = height - 250;

                         document.write("<iframe style='overflow:auto' name='apps' width='245' height='" + height + "' src='$SCRIPT_NAME?action=app_list'></iframe>");
                    }

                </script>

              </head>

              <body style='overflow:hidden;margin-top:5px;margin-left:0px;margin-right:0px;background:silver'>

                  <table summary='x1' width='100%' cellspacing='0' cellpadding='3'>

                       <tr>
                          <!-- LEFT MENU: Base -->
                          <td valign='top' nowrap='nowrap' width='220'>
                                <script language='javascript'>draw_utilities_frame();</script>
                          </td>

                          <!-- RIGHT SIDE -->
                          <td rowspan='2' width='100%' valign='top'>
                              <table width='100%' summary='x1' cellspacing='0' cellpadding='0'>
                                  <!-- Blue Frame -->
                                  <tr>
                                      <td>
                                         <iframe style='border:0px;' scrolling='no' name='blueframe' width='100%' height='35' src='$SCRIPT_NAME?action=blue_frame'></iframe>
                                      </td>
                                 </tr>

                                 <!-- Work Frame -->
                                 <tr>
                                      <td>
                                         <script language='javascript'>draw_bottom_frame();</script>
                                      </td>
                                  </tr>

                              </table>
                          </td>
                       </tr>

                       <tr>
                          <!-- LEFT MENU: Apps -->
                          <td valign='top' nowrap='nowrap' width='245'>
                                <script language='javascript'>draw_app_frame();</script>
                          </td>
                       </td>

                  </table>

              </body>

           </html>


END;
				}

				function draw_frames_simple ()
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $CGI;
						global $ADMIN_TYPE;
						echo <<<END

           <html>
              <head>
                  <title>$config[admin_title]</title>

                  <script language='javascript'>

                    var MSIE = (document.all) ? 1 : 0;

                    function openHelp( admin, action ) {
                        var Loc = '$config[html_dir]' + '/help.php?admin=' + admin + '&action=' + action;
                        help=open('', 'help','fullscreen=yes');
                        help.moveTo(0,0);
                        help.resizeTo(screen.width,screen.height);
                        help.document.location.href=Loc;
                        help.focus();
                    }

                    function draw_bottom_frame() {

                         var height = 640;

                         if (parseInt(navigator.appVersion)>3) {
                             height = window.innerHeight;
                             if ( MSIE ) { height = parent.document.body.offsetHeight; }
                         }

                         height = height - 70;

                         document.write("<iframe scrolling='auto' name='main' width='100%' height='" + height + "' src='$SCRIPT_NAME?action=main_frame'></iframe>");
                    }

                    function draw_utilities_frame() {

                         var height = 640;

                         if (parseInt(navigator.appVersion)>3) {
                             height = window.innerHeight;
                             if ( MSIE ) { height = parent.document.body.offsetHeight; }
                         }

                         height = height - 70;

                         document.write("<iframe scrolling='auto' name='util' width='100%' height='" + height + "' src='$SCRIPT_NAME?action=config_list'></iframe>");

                    }

                    function draw_app_frame() {

                         var height = 640;

                         if (parseInt(navigator.appVersion)>3) {
                             height = window.innerHeight;
                             if ( MSIE ) { height = parent.document.body.offsetHeight; }
                         }

                         height = height - 250;

                         document.write("<iframe style='overflow:auto' name='apps' width='245' height='" + height + "' src='$SCRIPT_NAME?action=app_list'></iframe>");
                    }

                </script>

              </head>

              <body style='overflow:hidden;margin-top:5px;margin-left:0px;margin-right:0px;background:silver'>

                  <table summary='x1' width='100%' cellspacing='0' cellpadding='3'>

                       <tr>
                          <!-- LEFT MENU: Base -->
                          <td valign='top' nowrap='nowrap' width='220'>
                                <script language='javascript'>draw_utilities_frame();</script>
                          </td>

                          <!-- RIGHT SIDE -->
                          <td width='100%' valign='top'>
                              <table width='100%' summary='x1' cellspacing='0' cellpadding='0'>
                                  <!-- Blue Frame -->
                                  <tr>
                                      <td>
                                         <iframe style='border:0px;' scrolling='no' name='blueframe' width='100%' height='35' src='$SCRIPT_NAME?action=blue_frame'></iframe>
                                      </td>
                                 </tr>

                                 <!-- Work Frame -->
                                 <tr>
                                      <td>
                                         <script language='javascript'>draw_bottom_frame();</script>
                                      </td>
                                  </tr>

                              </table>
                          </td>
                       </tr>

                  </table>

              </body>

           </html>


END;
				}

				function blue_frame ()
				{
						global $CGI;
						echo "
           <body style='margin:0px; padding:0px; border:1px solid silver;background:silver'>
              <div id='bluecontent'>
              <table width=100% hspace=0 vspace=0>
                <tr>
                  <td valign='top' align='left' style='font-family:verdana,arial,helvetica; font-size:9pt; font-weight:bold;color:navy'>
                    $CGI[message]
                  </td>
                </tr>
              </table>
              </div>
           </body>
       ";
						exit;
				}

				function main_frame ( $message = '' )
				{
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $HTTP_HOST;
						global $data_dir;
						global $include_dir;
						global $LICENSE_TYPE;
						global $ADMIN_TYPE;
						global $fatal_system_errors;
						$patches     = '';
						$patchlist   = '';
						$queue       = '';
						$admin_notes = '';
						$versions    = '';
						$patchcount  = 0;
						$completed   = array ();
						$files       = array ();
						if ( $CGI[ 'hide_message' ] ) {
								write_file ( "$data_dir/.hidemessage" , "x" );
						}
						if ( $CGI[ 'show_message' ] ) {
								unlink ( "$data_dir/.hidemessage" );
						}
						if ( !file_exists ( "$data_dir/.hidemessage" ) ) {
								$newusermessage = "
              <table width='500' cellspacing='0' cellpadding='3'>
                 <tr>
                    <td style='color:black; background: #e0e0e0; border:1px outset silver'>
                       <b>Getting Started With Hyperseek!</b>
                    </td>
                 </tr>

                 <tr>
                     <td style='background:white;border:1px solid black; border-bottom:0px'>
                         <table cellspacing='8' width='100%'>
                             <tr>
                                <td valign='top' nowrap='nowrap'><b>Find the Documentation</b></td>
                                <td>
                                   <a href='http://www.hyperseek.com/help/hyperseek/' target='new'>Online Version</a>
                                   <br />
                                   <i>If you installed using the Windows Based Installer, you also have a copy
                                   on your PC, under 'Hyperseek' on your 'Start Menu'</i>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                             <tr>
                                <td valign='top' nowrap='nowrap'><b>Know Where/how to get help</b></td>
                                <td>
                                   <a href='http://www.hyperseek.com/newsgroups/' target='new'>Interactive Newsgroups</a>
                                   <br />
                                   <a href='http://www.hyperseek.com/know/' target='new'>Knowledge Base</a>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                             <tr>
                                <td valign='top' nowrap='nowrap'><b>Set Your Preferences</b></td>
                                <td>
                                   <a href='$SCRIPT_NAME?action=config'>Options Screen</a>
                                   <br />
                                   <i>
                                     Hyperseek is shipped with a 'turnkey' set of preferences by default.
                                     The system is setup to show 20 results per page, all click through
                                     fraud checks are on, every submission must be approved, etc.  You should
                                     be able to hit the ground running with the default settings.  If you
                                     wish to change behavior, every setting in the preferences area has
                                     pop-up help to assist you in setting it properly.
                                   </i>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                            <tr>
                                <td valign='top' nowrap='nowrap'><b>Edit Your Language Settings</b></td>
                                <td>
                                   <a href='$SCRIPT_NAME?action=language'>Language Editor</a>
                                   <br />
                                   <i>
                                     Most (about 90%) of the words that you see in pages generated by Hyperseek
                                     are stored in 'Language Values'.  Things like the word 'search' on the
                                     search buttons, the next/previous links, etc. are all setup in the languages
                                     area.  Pretty much anything that you would like to change, wording-wise, can
                                     be changed here.
                                   </i>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                            <tr>
                                <td valign='top' nowrap='nowrap'><b>Edit Your Templates</b></td>
                                <td>
                                   <a href='$SCRIPT_NAME?action=templates'>Template Manager</a>
                                   <br /><br />
                                   <i>
                                     Hyperseek uses 2 types of templates to create your search engine pages....
                                     <br /><br />
                                     <a href='http://hyperseek.com/help/hyperseek/howpagesaredrawn.htm' target='new'>Full Page Templates</a> use <a href='http://hyperseek.com/help/hyperseek/whatisaplugin.htm' target='new'>Plugins</a> to place Hyperseek's output into your HTML design.
                                     <br /><br />
                                     <a href='http://hyperseek.com/help/hyperseek/internaldetailtemplates.htm' target='new'>Internal Templates</a> are used by Hyperseek within the various page plugins to tailor the bits and pieces (such as how an individual search result should look) that the plugins use when creating a page.  Internal templates also contain the text that Hyperseek uses on the emails that it sends out to your users.  You'll definitely want to look at, and change the wording on those.
                                   </i>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                            <tr>
                                <td valign='top' nowrap='nowrap'><b>Setup Your Category Structure</b></td>
                                <td>
                                   <a href='$SCRIPT_NAME?action=directory&admin=hyperseek'>Directory Manager</a>
                                   <br />
                                   <i>
                                     If you plan on offering your users a 'Yahoo' style category navigation structure, you'll need to start creating your category 'tree'.
                                   </i>
                                </td>
                             </tr>

                             <tr><td colspan='2' style='border-bottom:1px solid black'>&nbsp;</td></tr>

                         </table>
                     </td>
                 </tr>

                 <tr>
                     <td align='right' style='background:white;border:1px solid black; border-top:0px'>
                          <a href='$SCRIPT_NAME?action=main_frame&hide_message=1'>Hide this message</a>
                     </td>
                 </tr>

              </table>

           ";
						} else {
								$newusermessage = "
              <a href='$SCRIPT_NAME?action=main_frame&show_message=1'>View Getting Started Guide</a>
           ";
						}
						// Get version information //
						//$version_info = $this->versions(1);
						//$versions = "
						//      <fieldset style='padding:5px;font-family:verdana,arial; font-size:9pt;'>
						//        <legend style='font-size:12px; font-weight:bold;'>Version Information</legend>
						//          $version_info
						//      </fieldset>
						//";
						// Get Queue Information from HS, if it exists //
						if ( function_exists ( "queue_window" ) ) {
								$queue = queue_window ( 1 );
						} elseif ( file_exists ( "$include_dir/dsX/Admin/hyperseek.inc" ) ) {
								include_once ( "dsX/Admin/hyperseek.inc" );
								$queue = queue_window ( 1 );
						}
						// SQL Patches
						if ( $handle = opendir ( "$data_dir/sql/schemas" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( preg_match ( "/\.done/i" , $file ) ) {
												$file = preg_replace ( "/\.done/i" , "" , $file );
												$completed[ $file ]++;
										}
										if ( preg_match ( "/\.done/i" , $file ) ) {
												continue;
										}
										if ( !preg_match ( "/\.patch/i" , $file ) ) {
												continue;
										}
										array_push ( $files , $file );
								}
								closedir ( $handle );
						}
						foreach ( $files as $file ) {
								if ( $completed[ $file ] ) {
										if ( file_exists ( "$data_dir/sql/schemas/$file" ) ) {
												unlink ( "$data_dir/sql/schemas/$file" );
										}
										continue;
								}
								$patchcount++;
								$patchname = preg_replace ( "/\./" , ": " , $file );
								$patchlist .= " <li><a href='$SCRIPT_NAME?action=apply_patch&patch_name=$file'>$patchname</a><p> ";
						}
						if ( $patchcount <= 0 ) {
								$patchlist = "None";
						}
						$patches = "
               <fieldset style='padding:5px;font-family:verdana,arial; font-size:9pt;'>
                 <legend style='font-size:12px; font-weight:bold;'>SQL Patches</legend>
                   <b>The following SQL Database Patches need to be run...</b>
                   <ul> $patchlist </ul>
               </fieldset>
           ";
						// Notes //
						$notes       = implode ( "" , file ( "$data_dir/notes.txt" ) );
						$line        = str_replace ( "\r" , '' , $line );
						$line        = str_replace ( "\cM" , '' , $line );
						$notes       = str_replace ( "\n" , '<br />' , $notes );
						$date        = date ( "F d, Y h:i a" );
						$admin_notes = "
              <fieldset style='padding:5px;font-family:verdana,arial; font-size:9pt;'>
                <legend style='font-size:12px; font-weight:bold;'>$language[notes_prompt]</legend>
                  $notes
                  <br /><br />
                  <center><b><a href='$SCRIPT_NAME?action=edit_notes'>Edit/Add</a></b></center>
              </fieldset>
           ";
						// URLS //
						$base     = "http://" . $HTTP_HOST . $SCRIPT_NAME;
						$base     = preg_replace ( "/admin.php/i" , "" , $base );
						$search   = $config['search_url'] . "search.php";
						$accounts = $config['jackhammer_accounts_url'] . "accounts.php";
						$signup   = $config['jackhammer_accounts_url'] . "signup/";
						$urls     = "
              <fieldset style='padding:5px;font-family:verdana,arial; font-size:9pt;'>
                <legend style='font-size:12px; font-weight:bold;'>Links</legend>
                  <a href='$signup' target='new'>Advertiser Signup</a>
                  <br />
                  <a href='$accounts' target='new'>Advertiser Login</a>
                  <br />
                  <a href='$search' target='new'>Search Home</a>
                  <br />
                  <a href='$search?fmt=xml' target='new'>XML Feed</a>
                  <br />
                  <a href='$search?fmt=inline' target='new'>INLINE Feed</a>
              </fieldset>
           ";
						// Globally screen certain messages based on admin type //
						if ( !preg_match ( "/administrator/i" , $ADMIN_TYPE ) ) {
								$versions = '';
								$patches  = '';
						}
						if ( preg_match ( "/design/i" , $ADMIN_TYPE ) ) {
								$queue = '';
						}
						if ( count ( $fatal_system_errors ) >= 1 ) {
								$fatals  = join ( '<br />' , $fatal_system_errors );
								$message = "
             <div style='text-align:left; padding:10px; border:1px solid black;'>
                 <h1 style='color:red'>Fatal System Startup Error!</h1>
                 <blockquote><i>$fatals</i></blockquote>
                 <b>It is possible that there is a patch that needs to be applied. (listed below)
                 <br /><br />
                 $patches
                 <br /><br />
                 If the problem perists, please
                 <a href='http://www.hyperseek.com/newsgroups/' target='new'>contact iWeb Support immediately.</a>
             </div>
           ";
								admin_output ( "Fatal Error" , $message );
								exit;
						}
						$OUT = "
               <h3 style='color:blue'>$date</h3>
               <center><i>$message</i></center>


               <table cellspacing='20'>
                 <tr>
                   <td colspan='2'>
                     $newusermessage
                   </td>
                 </tr>
                 <tr>
                   <td valign='top'>
                      $urls
                   </td>
                   <td valign='top'>
                      $versions
                   </td>
                 </tr>
                 <tr>
                   <td valign='top'>
                      $queue
                   </td>
                   <td valign='top'>
                      $patches
                   </td>
                 </tr>
                 <tr>
                   <td colspan='2'>
                      $admin_notes
                   </td>
                 </tr>
               </table>
       ";
						admin_output ( $language[ welcome_header ] , $OUT );
						exit;
				}



				// --------------------------------------------------------------------- //
				//  Admin Menu Trees
				// --------------------------------------------------------------------- //
				function app_list ()
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $include_dir;
						global $PROGRAMS;
						global $menuitems;
						global $data_dir;
						$dir = $include_dir . "/dsX/Admin/config/";
						if ( is_dir ( $dir ) ) {
								$PROGRAMS = array ();
								if ( $handle = opendir ( $dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$file              = preg_replace ( "/\.inc/i" , "" , $file );
														$PROGRAMS[ $file ] = 1;
														include_once ( "dsX/Admin/config/$file.inc" );
												}
										}
										closedir ( $handle );
								}
						}
						// Catch any Custom Admin Centers
						$custom_admin_dir = $data_dir . "/admin";
						if ( is_dir ( $custom_admin_dir ) ) {
								if ( $handle = opendir ( $custom_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$file              = preg_replace ( "/\.inc/i" , "" , $file );
														$PROGRAMS[ $file ] = 1;
														include_once ( "$data_dir/admin/$file.inc" );
												}
										}
										closedir ( $handle );
								}
						}
						foreach ( $PROGRAMS as $program => $x ) {
								$function = "config_" . $program;
								call_user_func ( $function );
						}
						$admins     = '';
						$subs       = '';
						$items      = '';
						$num_admins = 0;
						if ( !$menuitems ) {
								echo "No Admin Apps ($dir)";
								exit;
						}
						ksort ( $menuitems );
						foreach ( $menuitems as $key => $array ) {
								$num_admins++;
								$varname = "admin_" . $num_admins;
								$name    = $menuitems[ $key ][ title ];
								$admins .= "
                var $varname = new WebFXTreeItem(' $name');
                tree.add($varname);
            ";
								$num_cats = 0;
								foreach ( $menuitems[ $key ][ categories ] as $category => $array ) {
										$num_cats++;
										$cat_varname = "admincat_" . $num_admins . "_" . $num_cats;
										$menu_item   = " var $cat_varname = new WebFXTreeItem(' $category'); ";
										if ( preg_match ( "/\//" , $menuitems[ $key ][ categories ][ $category ] ) ) {
												$url       = $menuitems[ $key ][ categories ][ $category ];
												$menu_item = " var $cat_varname = new WebFXTreeItem(' $category','$url'); ";
										}
										$subs .= "
                    $menu_item
                    $varname.add($cat_varname);
                ";
										$num_items = 0;
										if ( $menuitems[ $key ][ items ][ $category ] ) {
												ksort ( $menuitems[ $key ][ items ][ $category ] );
												foreach ( $menuitems[ $key ][ items ][ $category ] as $item => $array ) {
														// foreach my $item ( sort keys %{$menuitems{$key}{items}{$category}} ) {
														$num_items++;
														$item_varname = "admincat_" . $num_admins . "_" . $num_cats . "_" . $num_items;
														$url          = $menuitems[ $key ][ items ][ $category ][ $item ];
														$items .= "
                            var $item_varname = new WebFXTreeItem(' $item','$url');
                            $cat_varname.add($item_varname);
                        ";
												}
										}
								}
						}
						$num_oa = 0;
						if ( file_exists ( "$data_dir/otheradmin.txt" ) ) {
								$oa = file ( "$data_dir/otheradmin.txt" );
								foreach ( $oa as $file ) {
										$num_oa++;
										list ( $title , $url ) = split ( "==" , $file );
										$url       = preg_replace ( "/\n|\r|\cM/" , "" , $url );
										$showtitle = "otheradmin_" . $num_oa;
										$admins .= "
                    var $showtitle = new WebFXTreeItem(' $title', '$url');
                    tree.add($showtitle);
                ";
								}
						}
						$title     = "<b>Applications</b>";
						$tree_vars = "
            $admins
            $subs
            $items
        ";
						$this->draw_admin_tree ( $title , $tree_vars );
				}

				// Application List Specific for certain admin types //
				function custom_app_list ()
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $include_dir;
						global $PROGRAMS;
						global $menuitems;
						global $ADMIN_TYPE;
						$dir = $include_dir . "/dsX/Admin/config/";
						if ( is_dir ( $dir ) ) {
								$PROGRAMS = array ();
								if ( $handle = opendir ( $dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$file              = preg_replace ( "/\.inc/i" , "" , $file );
														$PROGRAMS[ $file ] = 1;
														include_once ( "dsX/Admin/config/$file.inc" );
												}
										}
										closedir ( $handle );
								}
						}
						foreach ( $PROGRAMS as $program => $x ) {
								$function = "config_" . $program;
								call_user_func ( $function );
						}
						$admin_links = array ();
						if ( !$menuitems ) {
								return;
						}
						ksort ( $menuitems );
						foreach ( $menuitems as $key => $array ) {
								foreach ( $array[ 'items' ] as $itemarray ) {
										foreach ( $itemarray as $title => $url ) {
												if ( preg_match ( "/\//" , $url ) ) {
														$admin_links[ $key ][ $title ] = $url;
												}
										}
								}
						}
						$LINKS = "";
						ksort ( $admin_links );
						foreach ( $admin_links as $area => $links ) {
								$LINKS .= "<br /><b>$area</b><br />";
								foreach ( $links as $title => $url ) {
										$LINKS .= "&#183;&nbsp; <a href='$url' target='main'>$title</a><br />\n";
								}
						}

						return "
          <br /><br />
          <b>System Utilities</b>
          <div style='padding-left:5px'>
          $LINKS
          </div>
        ";
				}

				// --------------------------------------------------------------------- //
				//  Top Left Configuration (Basic Admin) Links
				// --------------------------------------------------------------------- //
				function config_list ()
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $ADMIN_TYPE;
						$menu            = "";
						$language_link   = "<a href='$SCRIPT_NAME?action=language' target='main'>$language[lang_link]</a><br />";
						$plugins_link    = "<a href='$SCRIPT_NAME?action=plugins' target='main'>$language[plugins_link]</a><br />";
						$versions_link   = "<a href='$SCRIPT_NAME?action=versions' target='main'>$language[versions_link]</a><br />";
						$backup_link     = "<a href='$SCRIPT_NAME?action=backup' target='main'>$language[backup_link]</a><br />";
						$templates_link  = "<a href='$SCRIPT_NAME?action=templates' target='main'>$language[templates_link]</a><br />";
						$serverinfo_link = "<a href='$SCRIPT_NAME?action=serverinfo' target='main'>$language[moduletest_link]</a>";
						if ( $ADMIN_TYPE == "designer" ) {
								$menu = $language_link . $plugins_link . $templates_link;
						} else if ( $ADMIN_TYPE == "administrator" ) {
								$menu = $language_link . $plugins_link . $templates_link . $versions_link . $backup_link . $serverinfo_link;
						} else {
								$menu = $this->custom_app_list ();
						}
						$output = "
            <style>
              a { text-decoration: none; color:navy; font-weight:bold; }
              a:hover { text-decoration: none; font-weight:normal; color:blue; }
            </style>
            <body style='margin:0px; padding:0px; border:1px solid silver;background:silver;overflow:hidden'>
                <table width='100%'>
                  <tr>
                    <td style='background: #414141 url($config[button_dir]/appheader.jpg);background-repeat: no-repeat;font-family:arial;font-size:14px;color:white;font-weight:bold;'>
                       Admin Tools ... ($ADMIN_TYPE)
                    </td>
                  </tr>
                  <tr>
                    <td>
                       <fieldset style='width:100%;padding:3px;font-family:arial;font-size:11px;font-weight:bold'>

                         <a href='$SCRIPT_NAME?action=main_frame' target='main'>$language[home_link]</a><br />
                         <a href='$SCRIPT_NAME?action=config' target='main'>$language[config_link]</a><br />

                         $menu

                         <br /><br />
                         <center>
                            <a style='color:blue' href='$SCRIPT_NAME?action=support' target='main'>[help]</a>
                         </center>
                       </fieldset>
                    </td>
                  </tr>
                </table>
             </body>
        ";
						echo $output;
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Set Configuration Options
				// --------------------------------------------------------------------- //
				function config ( $message = '' )
				{
						global $data_dir;
						global $language;
						global $config;
						global $CGI;
						global $SCRIPT_NAME;
						global $include_dir;
						global $PROGRAMS;
						global $vars;
						global $ADMIN_TYPE;
						// Generate Top Menu of configured "themes"
						$themes = array ();
						if ( $handle = opendir ( "$data_dir/templates" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( $file == "CVS" || $file == "." || $file == ".." ) {
												continue;
										}
										$themes[ $file ] = $file;
								}
								closedir ( $handle );
						}
						$themelist = "";
						if ( !$CGI[ theme ] ) {
								$CGI[ theme ] = "default";
						}
						asort ( $themes );
						foreach ( $themes as $theme ) {
								if ( $themelist ) {
										$themelist .= " | ";
								}
								if ( $CGI[ theme ] == $theme ) {
										$themelist .= "<B>$theme</B>";
								} else {
										$themelist .= " <a href='$SCRIPT_NAME?action=config&theme=$theme'>$theme</a> ";
								}
						}
						// Generate List of modules with config options
						$options        = "<tr><td colspan=2><blockquote>\n";
						$base_admin_dir = $include_dir . "/dsX/Admin/config/";
						if ( is_dir ( $base_admin_dir ) ) {
								$PROGRAMS = array ();
								if ( $handle = opendir ( $base_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$appname = preg_replace ( "/\.inc/i" , "" , $file );
														// $PROGRAMS[$file] = 1;
														$fname = $base_admin_dir . $file;
														include_once ( $fname );
														$setting_func = "config_" . $appname;
														call_user_func ( $setting_func );
												}
										}
										closedir ( $handle );
								}
						} else {
								foreach ( $PROGRAMS as $prog => $stuff ) {
										$setting_func = "config_" . $prog;
										call_user_func ( $setting_func );
								}
						}
						// This will catch any custom or added on admin settings areas
						$custom_admin_dir = $data_dir . "/admin/";
						if ( is_dir ( $custom_admin_dir ) ) {
								if ( $handle = opendir ( $custom_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$appname = preg_replace ( "/\.inc/" , "" , $file );
														$fname   = $custom_admin_dir . $file;
														include_once ( $fname );
														$setting_func = "config_" . $appname;
														call_user_func ( $setting_func );
												}
										}
										closedir ( $handle );
								}
						}
						ksort ( $vars );
						foreach ( $vars as $configtype => $stuff ) {
								if ( preg_match ( "/templates/" , $configtype ) ) {
										continue;
								}
								if ( !empty( $CGI[ 'theme' ] ) && $CGI[ 'theme' ] != "default" && $configtype != "Style" ) {
										continue;
								}
								if ( !empty( $language[ $configtype ] ) ) {
										$prompt = $language[ $configtype ];
								} else {
										$prompt = preg_replace ( '/\_/' , " " , $configtype );
										$prompt = ucwords ( $prompt );
								}
								$options .= "
                  <li><a href='$SCRIPT_NAME?action=edit_options&option=$configtype&theme=$CGI[theme]' target='main'>$prompt</a><br />
               ";
						}
						$options .= "</td></tr>\n";
						$OUT = "

                    <script language='javascript'>

                      function Help(c_what) {
                        var Location = '$config[html_dir]/options.php?help_topic=' + c_what;
                        link=open(Location,'CCSLink','toolbar=no,scrollbars=yes,directories=no,menubar=no,resizable=yes,width=420,height=410');
                        link.focus();
                      }

                    </script>

                    $message

                    Theme: $themelist


                    <form name='var_form' action='$SCRIPT_NAME' method='POST'>
                    <font size=+2><b><i>$language[options_header]</i></b></font>
                    <table border=0 cellspacing=3 cellpadding=5>
                         $options
                    </table>
                    </form>
             ";
						admin_output ( "Program Configuration" , $OUT );
				}


				// --------------------------------------------------------------------- //
				//  Configuration Editing
				// --------------------------------------------------------------------- //
				function edit_options ()
				{
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $data_dir;
						global $include_dir;
						global $vars;
						global $ADMIN_TYPE;
						global $PROGRAMS;
						$designer_mode = 0;
						if ( preg_match ( "/design/" , $SCRIPT_NAME ) ) {
								$designer_mode = 1;
						}
						$setting = $CGI[ 'option' ];
						// If this is a differen theme, read in it's file, overwriting the default config settings
						if ( $CGI[ theme ] != "default" ) {
								$file = "$data_dir/config/$CGI[theme].inc";
								if ( file_exists ( $file ) ) {
										$config = array ();
										include ( $file );
								}
						}
						// Read in all of the options, saving out just the option needed //
						$base_admin_dir = $include_dir . "/dsX/Admin/config/";
						if ( is_dir ( $base_admin_dir ) ) {
								$PROGRAMS = array ();
								if ( $handle = opendir ( $base_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$appname = preg_replace ( "/\.inc/" , "" , $file );
														// $PROGRAMS[$file] = 1;
														$fname = $base_admin_dir . $file;
														include_once ( $fname );
														$setting_func = "config_" . $appname;
														call_user_func ( $setting_func );
												}
										}
										closedir ( $handle );
								}
						} else {
								foreach ( $PROGRAMS as $prog => $stuff ) {
										$setting_func = "config_" . $prog;
										call_user_func ( $setting_func );
								}
						}
						// This will catch any custom or added on admin settings areas
						$custom_admin_dir = $data_dir . "/admin/";
						if ( is_dir ( $custom_admin_dir ) ) {
								if ( $handle = opendir ( $custom_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$appname = preg_replace ( "/\.inc/" , "" , $file );
														$fname   = $custom_admin_dir . $file;
														include_once ( $fname );
														$setting_func = "config_" . $appname;
														call_user_func ( $setting_func );
												}
										}
										closedir ( $handle );
								}
						}
						$options = "";
						ksort ( $vars[ $setting ] );
						foreach ( $vars[ $setting ] as $varname => $array ) {
								list ( $prompt , $type , $default , $mode ) = $array;
								if ( $ADMIN_TYPE != 'administrator' ) {
										if ( $ADMIN_TYPE == "designer" && $mode != "design" ) {
												continue;
										} else if ( $ADMIN_TYPE == "content" && $mode != "content" ) {
												continue;
										} else if ( $ADMIN_TYPE == "affiliate" && $mode != "affiliate" ) {
												continue;
										}
								}
								$varname = preg_replace ( "/.*\:/" , "" , $varname );
								if ( !$config{$varname} && preg_match ( "/text/i" , $type ) ) {
										$config[ $varname ] = $default;
								}
								if ( $type == "TEXT" ) {
										$userinput = " <input name='$varname' value='$config[$varname]' size='40'> ";
								} elseif ( $type == "TEXTAREA" ) {
										$userinput = " <textarea name='$varname' rows='5' cols='40'>$config[$varname]</textarea> ";
								} elseif ( $type == "CHECKBOX" ) {
										$userinput = " <input type='checkbox' name='$varname' $config[$varname]>Yes";
								} elseif ( $type == "SELECT" || $type == "MULTIPLE" ) {
										if ( $type == "MULTIPLE" ) {
												$multi = " multiple size='5'";
										} else {
												$multi = "";
										}
										$userinput = " <select name='$varname' $multi> ";
										if ( !$multi ) {
												$userinput .= " <option value='$config[$varname]' selected>$config[$varname]</option> ";
										}
										if ( preg_match ( "/generate/i" , $default ) ) {
												list ( $x , $y ) = split ( '\:' , $default );
												if ( method_exists ( $this , $y ) ) {
														$x = call_user_func ( array ( &$this , $y ) );
												} elseif ( function_exists ( $y ) ) {
														$x = call_user_func ( $y );
												}
												$userinput .= $x;
										} else {
												foreach ( split ( '\|' , $default ) as $option ) {
														$eoption = $option;
														if ( $type == "MULTIPLE" && preg_match ( "/" . $option . "/i" , $config[ $varname ] ) ) {
																$selected = "SELECTED";
														} elseif ( $type == "SELECT" && $config[ $varname ] == $eoption ) {
																$selected = "SELECTED";
														} else {
																$selected = "";
														}
														$userinput .= " <option value='$option' $selected>$option</option> ";
												}
										}
										$userinput .= "</select>";
								}
								$help = "
              <a style='text-decoration:none;color:blue;font-size:14px;font-weight:bold' href='javascript:Help(\"$varname\");'>?</a>
           ";
								if ( $type == "HEADER" ) {
										$options .= " <tr><td colspan='2' style='background:#414141; color:white'><b>$prompt</b></td></tr> ";
								} else {
										$options .= "
               <tr style='background:#e0e0e0'>
                 <td valign='top'>$help $prompt</td>
                 <td valign='top'>$userinput</td>
               </tr>
              ";
								}
						}
						$options .= "
           <tr style='background:#e0e0e0'>
             <td colspan=2>
              <center><input type='submit' value='$language[save_options_button]'>
              <input type='hidden' name='action' value='save_options'>
              <input type='hidden' name='password' value='$config[admin_pw]'>
              <input type='hidden' name='theme' value='$CGI[theme]'>
              </center>
             </td>
           </tr>
        ";
						if ( !empty( $language[ $setting ] ) ) {
								$prompt = $language[ $setting ];
						} else {
								$prompt = preg_replace ( '/\_/' , " " , $setting );
								$prompt = ucwords ( $prompt );
						}
						$OUT = "
            <script language='javascript'>
               function Help(c_what) {
                 var Location = '$config[html_dir]/options.php?help_topic=' + c_what;
                 link=open(Location,'CCSLink','toolbar=no,scrollbars=yes,directories=no,menubar=no,resizable=yes,width=420,height=410');
                 link.focus();
                }
            </script>
              <br />
              <form name='var_form' action='$script_name' method='post' enctype='application/x-www-form-urlencoded'>
              <table border=0 bgcolor='#e0e0e0' cellspacing=0 cellpadding=0 align='center' width='85%'>
               <tr><td valign='top'>
                 <table border=0 width='100%' bgcolor='#000000' cellspacing=1 cellpadding=5 align='center'>
                   <tr>
                     <td colspan=2 style='background:navy; color:white'><b>Configuration Settings: <I>$prompt</I></b></td>
                   </tr>
                   $options
                 </table>
               </td></tr>
              </table>
              </font>
              </form>
         ";
						admin_output ( "Program Configuration" , $OUT );
				}

				// --------------------------------------------------------------------- //
				//  Helper Functions (Generators)
				// --------------------------------------------------------------------- //
				function save_options ()
				{
						global $CGI;
						global $data_dir;
						global $config;
						global $LICENSE_TYPE;
						// Prevent lite and demo users from turning off backfill //
						if ( !preg_match ( "/unlimited|source/i" , $LICENSE_TYPE ) ) {
								if ( !empty( $CGI[ 'supplemental_results' ] ) && $CGI[ 'supplemental_results' ] == "NO" ) {
										$message = "As you are a user of the Hyperseek $LICENSE_TYPE version, you are not permitted to turn off the XML Backfill Option";
										$this->config ( "<div style='width:400px;padding:5px; border:1px solid black; background:ivory'>$message</div><br /><br />" );
										exit;
								}
						}
						// Start Clean //
						$config = array ();
						// If we're in a theme, update it, and get out //
						if ( $CGI[ theme ] != "default" ) {
								$themefile = "$data_dir/config/$CGI[theme].inc";
								include ( $themefile );
								foreach ( $CGI as $field_name => $val ) {
										$field_name = preg_replace ( "/\n|\r|\cM/" , "" , $field_name );
										$val        = preg_replace ( "/\n|\r|\cM/" , "" , $val );
										if ( preg_match ( "/password|theme|action/i" , $field_name ) ) {
												continue;
										}
										$config[ $field_name ] = $val;
								}
								$dump          = var_export ( $config , 1 );
								$save_settings = "<?php\n\n";
								$save_settings .= "\$config = \n";
								$save_settings .= $dump;
								$save_settings .= "\n\n?>";
								copy ( $themefile , $themefile . '.save' );
								list ( $status , $message ) = write_file ( $themefile , $save_settings );
						} else {
								$config_file = "$data_dir/config.inc";
								include ( $config_file );
								foreach ( $CGI as $field_name => $val ) {
										$field_name = preg_replace ( "/\n|\r|\cM/" , "" , $field_name );
										$val        = preg_replace ( "/\n|\r|\cM/" , "" , $val );
										if ( preg_match ( "/password|theme|action/i" , $field_name ) ) {
												continue;
										}
										$config[ $field_name ] = $val;
								}
								$dump          = var_export ( $config , 1 );
								$save_settings = "<?php\n\n";
								$save_settings .= "\$config = \n";
								$save_settings .= $dump;
								$save_settings .= "\n\n?>";
								copy ( $config_file , $config_file . '.save' );
								if ( isset( $config[ 'config_sync_script' ] ) && !empty( $config[ 'config_sync_script' ] ) && strcasecmp ( $config[ 'config_sync_script' ] , "none" ) ) {
										exec ( "$data_dir/../../" . $config[ 'config_sync_script' ] );
								}
								list ( $status , $message ) = write_file ( $config_file , $save_settings );
						}
						$this->config ( "<div style='width:400px;padding:5px; border:1px solid black; background:ivory'>$message</div><br /><br />" );
						exit;
				}



				// --------------------------------------------------------------------- //
				//  Server Information
				// --------------------------------------------------------------------- //
				function serverinfo ()
				{
						phpinfo ();
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Language Menu
				// --------------------------------------------------------------------- //
				function language ()
				{
						global $config;
						global $language;
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						$defined_languages = '';
						$admin_sections    = '';
						$regular_sections  = '';
						$forms             = '';
						$new_language      = '';
						$new_set           = '';
						$SQL               = "SELECT DISTINCT language FROM " . LANGUAGE_TABLE;
						$langs             = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						foreach ( $langs as $lang ) {
								$defined_languages .= " <option value='$lang'>$lang</option> ";
						}
						$SQL     = "SELECT DISTINCT section FROM " . LANGUAGE_TABLE . " WHERE language = 'english'";
						$regular = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						foreach ( $regular as $section ) {
								if ( preg_match ( "/admin/i" , $section ) ) {
										$admin_sections .= " <option value='$section'>$section</option> ";
								} else {
										$regular_sections .= " <option value='$section'>$section</option> ";
								}
						}
						$rows = "
              <form action='$SCRIPT_NAME' method='post' target='frame-T1'>
                 $language[language_manager]
                 <select name='language'>
                    $defined_languages
                 </select>
                 <select name='section'>
                    <option value=''>-- Front End Definitions--</option>
                    <option value=''></option>
                    $regular_sections
                    <option value=''></option>
                    <option value=''>\n--Admin Definitions--\n</option>
                    <option value=''></option>
                    $admin_sections
                 </select>
                 <input type='hidden' name='action' value='edit_language' />
                 <input type='submit' value='Edit Language' />
              </form>
          ";
						## Table Form Layouts.
						$system  = array ( "acct_deposits" , "acct_transactions" , "affiliate_access" , "affiliate_payments" , "affiliate_searches" , "a_surveys" , "a_survey_detail" , "a_survey_questions" , "a_survey_results" , "bm_accounts" , "bm_click_log" , "bm_click_stats" , "bm_exchange_curr" , "bm_finances" , "bm_payments" , "bm_rotation" , "bm_sites_user" , "bm_statistics" , "campaigns_user" , "go_hits" , "hs_featured" , "hs_moderator_log" , "hs_optin" , "hs_related" , "hs_reviews" , "hs_subscribers" , "hs_templates" , "hs_validate" , "hyperseek_cats" , "hyperseek_forbid" , "hyperseek_keys" , "hyperseek_log" , "hyperseek_results" , "hyperseek_stats" , "ispyder_keys" , "ispyder_links" , "iweb_language" , "jh_bids_pending" , "nav_templates" , "sb_directories" , "sb_editors" , "sb_html" , "sb_keys" , "sb_links" , "sb_templates" , "sb_websites" , "survey_access" , "survey_detail" , "survey_results" , "survey_templates" , "table_def" );
						$in      = join ( "','" , $system );
						$SQL     = "SELECT distinct schema_name FROM " . SCHEMAS . " WHERE schema_name NOT IN('$in') ORDER BY schema_name";
						$schemas = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						foreach ( $schemas as $schema ) {
								$forms .= " <option value='$schema'>$schema</option> ";
						}
						if ( !$CGI[ language ] ) {
								$new_language = "
              <form action='$SCRIPT_NAME' action='post'>
                <b>Create New Language</b>: <input name='language'>
                <input type='hidden' name='action' value='new_language'>
                <b>Copy From:</b>
                <select name='copy_language'> $defined_languages </select>
                <input type='submit' value='Create It'>
              </form>
             ";
						} else {
								$new_set = "
               <form action='$SCRIPT_NAME' target='frame-T1'>
                  $language[new_language_set_prompt]
                     <input name='section'>
                     <input type='hidden' name='action' value='Edit_Language'>
                     <input type='hidden' name='language' value='$CGI[language]'>
                     <input type='submit' value='$language[new_language_set_button]'>
                     $language[new_language_set_help]
               </form>
             ";
						}
						if ( $CGI[ section ] ) {
								$admin_link = "
                : <a href='$SCRIPT_NAME?action=language&language=$CGI[language]&section=admin'>$language[language_admin_link]</a>
             ";
						}
						## END Data/Variable Setup ... draw the tabs ....
						## The titles for the admin tabs, in the order that they'll appear accross
						## the top of the tabset.
						// $tab_titles = array( $language[language_tab_translate], $language[language_tab_forms], $language[language_tab_html], $language[language_tab_glossary], $language[language_tab_synonym] );
						$tab_titles = array ( $language[ language_tab_translate ] , $language[ language_tab_forms ] , $language[ language_tab_synonym ] );
						## The tab contents for each of the above titles, note that
						## the titles and hash keys are the SAME.
						$tab_definition = array ( $language[ language_tab_translate ] => "
                           $rows
                           $new_language
                           $new_set
                       " , $language[ language_tab_forms ] => "
                               <font size=+1>$language[input_forms_prompt]</font><br />
                               <form action='$SCRIPT_NAME' target='frame-T2'>
                                     Select Schema: <select name='schema'>
                                     $forms
                                     </select>
                                     <input type='hidden' name='action' value='quick_edit'>
                                     <input type='hidden' name='admin' value='sql'>
                                     <input type='submit' value='Edit'>
                               </form>
                       " , /*

               $language[language_tab_html] =>
                       "
                               $language[html_substitution_info]
                               <a href='$SCRIPT_NAME?action=html_substitutions' target='frame-T3'>$language[html_substitution_link]</a>
                       ",

               $language[language_tab_glossary] =>
                       "
                               <form action='$SCRIPT_NAME' target='frame-T4'>
                                  Glossary Term: <input name='word'>
                                          <input type='hidden' name='action' value='glossary_search'>
                                          <input type='submit' value='$language[glossary_button]'>
                               </form>
                               <a href='$SCRIPT_NAME?action=glossary_search' TARGET='frame-T4'>$language[glossary_add_term]</a>
                       ",
*/
						                          $language[ language_tab_synonym ] => "
                               <form action='$SCRIPT_NAME' target='frame-T5'>
                                  Term: <input name='word'>
                                          <input type='hidden' name='action' value='lexical_search'>
                                          <input type='submit' value='$language[glossary_button]'>
                               </form>
                               <a href='$SCRIPT_NAME?action=lexical_search' TARGET='frame-T5'>$language[glossary_add_term]</a>
                       " , );
						$tabs           = $this->admin_tabs ( $tab_titles , $tab_definition );
						admin_output ( $language[ language_manager ] , $tabs );
				}


				// --------------------------------------------------------------------- //
				//  Edit a language section
				// --------------------------------------------------------------------- //
				function edit_language ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $SCRIPT_NAME;
						$SQL          = "SELECT * FROM " . LANGUAGE_TABLE . " WHERE language='$CGI[language]' AND section = '$CGI[section]'";
						$lang         = $oSQL->search ( array ( "SQL" => $SQL ) );
						$rows         = '';
						$hiddenfields = '';
						aasort ( $lang , "+setting_name" );
						foreach ( $lang as $setting ) {
								$show_var = $setting[ setting_name ];
								$show_var = preg_replace ( "/_/" , " " , $show_var );
								$input    = " <input name='$setting[setting_name]' value='$setting[current_value]'> ";
								if ( strlen ( $setting[ current_value ] ) >= 100 ) {
										$input = " <textarea name='$setting[setting_name]' rows='3' cols='50' wrap='virtual'>$setting[current_value]</textarea> ";
								}
								$rows .= "
                   <tr>
                     <td bgcolor='#efefef' valign='top' align='right'>
                       <a href='javascript:help_me(\"$SCRIPT_NAME?action=edit_language_record&language=$setting[language]&section=$setting[section]&setting_name=$setting[setting_name]\")'><b>$show_var</b></a>
                     </td>
                     <td valign='top' bgcolor='#efefef'>
                       $input
                     </td>
                   </tr>
                ";
								$hiddenfields .= "<input type='hidden' name='$setting[setting_name]' value='1'> ";
						}
						$output = "
                <script language='javascript'>
                   function help_me (location) {
                       x=window.open(location,'x','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=500,height=300')
                   }
                </script>

                       <br /><br />
                         <table border=0 bgcolor='black' cellspacing='1' cellpadding='5' align='center'>
                           <form name='lang_form' action='$SCRIPT_NAME' method='post'>

                           <tr bgcolor='silver'><th>$language[edit_lang_variable]</th><th>$language[edit_lang_value]</th></tr>

                           $rows

                           <tr>
                               <td align='right' bgcolor='#e0e0e0'>
                                 <b>$language[edit_lang_new]</b><br />
                                 $language[edit_lang_new_name] <input name='new_name'><br />
                                 $language[edit_lang_new_value] <input name='new_value'><br />
                                 $language[edit_lang_new_default]<input name='new_default'><br />
                                 $language[edit_lang_new_help] <input name='new_help'>

                                 <input type='hidden' name='language' value='$CGI[language]'>
                                 <input type='hidden' name='section' value='$CGI[section]'>
                                 <input type='hidden' name='action' value='save_language'>
                               </td>
                               <td bgcolor='#e0e0e0' align='center' valign='middle'>
                                 <input type='submit' value='$language[lang_save_changes]'>

                               </td>
                           </tr>

                           </form>

                         </table>

                         <center>
                             <form action = 'http://www.iwebsupport.com/applications/language.php' method='post'>
                                  <input type='submit' value='$language[language_verify]'>
                                  <input type='hidden' name='section' value='$CGI[section]'>
                                  <input type='hidden' name='module' value='$CGI[section]'>
                                  <input type='hidden' name='language' value='$CGI[language]'>
                                  $hiddenfields
                             </form>
                         </center>

           ";
						admin_output ( $language[ edit_lang_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  Save language section
				// --------------------------------------------------------------------- //
				function save_language ()
				{
						global $CGI;
						global $language;
						global $iSQL;
						global $data_dir;
						foreach ( $CGI as $field => $value ) {
								if ( preg_match ( "/section|language|new_name|new_default|new_help|new_value|action/" , $field ) ) {
										continue;
								}
								$lang = new Record( array ( "Parent" => $iSQL , "Table" => LANGUAGE_TABLE , "Schema" => LANGUAGE_TABLE , "Lookup" => 1 , "language" => $CGI[ language ] , "section" => $CGI[ section ] , "setting_name" => $field ) );
								if ( $value ) {
										$lang->data[ language ]      = $CGI[ language ];
										$lang->data[ section ]       = $CGI[ section ];
										$lang->data[ setting_name ]  = $field;
										$lang->data[ current_value ] = $value;
										$lang->update_values ( $lang->data );
										$lang->save ();
								} else {
										$lang->delete ();
								}
						}
						if ( $CGI[ new_name ] && $CGI[ new_value ] ) {
								$lang                        = new Record( array ( "Parent" => $iSQL , "Table" => LANGUAGE_TABLE , "Schema" => LANGUAGE_TABLE , "Lookup" => 1 , "language" => $CGI[ language ] , "section" => $CGI[ section ] , "setting_name" => $CGI[ new_name ] ) );
								$lang->data[ language ]      = $CGI[ language ];
								$lang->data[ section ]       = $CGI[ section ];
								$lang->data[ setting_name ]  = $CGI[ new_name ];
								$lang->data[ current_value ] = $CGI[ new_value ];
								$lang->update_values ( $lang->data );
								$lang->save ();
						}
						if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
								unlink ( "$data_dir/phpsettings.inc" );
						}
						admin_output ( $language{edit_lang_title} , "<b>$CGI[language]/$CGI[section]</b> $language[language_saved]" );
				}


				// --------------------------------------------------------------------- //
				//  Edit Individual Language variable
				// --------------------------------------------------------------------- //
				function edit_language_record ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						$SQL  = "SELECT * FROM " . LANGUAGE_TABLE . " WHERE language='$CGI[language]' AND section = '$CGI[section]' AND setting_name = '$CGI[setting_name]'";
						$lang = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
						echo "

             <form action='$SCRIPT_NAME' mehtod='post'>
             <table border=0 bgcolor='black' width=95% align='center'>
                <tr><td>
                   <table border=0 bgcolor='#e0e0e0' width=100%>
                      <tr bgcolor='navy'>
                         <td>
                           <font color='white' face='arial,helvetica' size=-1>
                              <b>$CGI[language] &#183; $CGI[section] &#183; $CGI[setting_name]</b>
                           </font>
                         </td>
                      </tr>
                      <tr>
                         <td>
                            <font face='arial,helvetica' size=-1>
                              <b>$language[lang_help_text]</b><br />
                              <i>$lang[help_text]</i><br />
                              <b>Current Value</b><br />
                                 <textarea name='current_value' rows=3 cols=40>$lang[current_value]</textarea>
                              <hr>
                              <b>$language[lang_default_value]</b><br />
                              <textarea name='default_value' rows=3 cols=40>$lang[default_value]</textarea>
                           </font>
                         </td>
                      </tr>
                      <tr>
                         <td colspan=2 align=center>
                              <input type='hidden' name='action' value='save_language_record'>
                              <input type='hidden' name='language' value='$CGI[language]'>
                              <input type='hidden' name='section' value='$CGI[section]'>
                              <input type='hidden' name='setting_name' value='$CGI[setting_name]'>
                              <input type='submit' value='$language[lang_save_changes]'>
                              <input type='button' onclick='self.close()' value='$language[lang_cancel]'>
                         </td>
                      </tr>
                   </table>
                </td></tr>
             </table>
             </form>

           ";
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Save single language variable
				// --------------------------------------------------------------------- //
				function save_language_record ()
				{
						global $CGI;
						global $language;
						global $iSQL;
						global $data_dir;
						$output                      = '';
						$lang                        = new Record( array ( "Parent" => $iSQL , "Table" => LANGUAGE_TABLE , "Schema" => LANGUAGE_TABLE , "Lookup" => 1 , "language" => $CGI[ language ] , "section" => $CGI[ section ] , "setting_name" => $CGI[ setting_name ] ) );
						$lang->data[ default_value ] = $CGI[ default_value ];
						$lang->data[ current_value ] = $CGI[ current_value ];
						if ( $lang->update_values ( $lang->data ) ) {
								if ( $lang->save () ) {
										$output = $language[ lang_default_saved ];
								} else {
										$output = "$language[lang_default_error] <pre>$lang->error_text</pre>";
								}
						} else {
								$output = "$language[lang_default_error] <pre>$lang->error_text</pre>";
						}
						if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
								unlink ( "$data_dir/phpsettings.inc" );
						}
						echo "

             <form>
             <table border=0 bgcolor='black' width=95% align='center'>
                <tr><td>
                   <table border=0 bgcolor='#e0e0e0' width=100%>
                      <tr bgcolor='navy'>
                         <td>
                           <font color='white' face='arial,helvetica' size=-1>
                              <b>$CGI[language]:$CGI[section]:$CGI[setting_name]</b>
                           </font>
                         </td>
                      </tr>
                      <tr>
                         <td>
                            <b>$output</b>
                            <center><input type='button' onclick='self.close()' value='$language[lang_done_button]'></center>
                         </td>
                      </tr>
                   </table>
                </td></tr>
             </table>
             </form>

           ";
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Copy Language Set from another existing one
				// --------------------------------------------------------------------- //
				function new_language ()
				{
						global $CGI;
						global $iSQL;
						$file        = '';
						$newlanguage = $CGI[ language ];
						$SQL         = "SELECT * FROM " . LANGUAGE_TABLE . " WHERE language = '$CGI[copy_language]'";
						$master      = $iSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $master as $field ) {
								$lang                        = new Record( array ( "Parent" => $iSQL , "Table" => LANGUAGE_TABLE , "Schema" => LANGUAGE_TABLE ) );
								$lang->data[ language ]      = $CGI[ language ];
								$lang->data[ section ]       = $field[ section ];
								$lang->data[ setting_name ]  = $field[ setting_name ];
								$lang->data[ default_value ] = $field[ default_value ];
								$lang->data[ current_value ] = $field[ current_value ];
								$lang->data[ help_text ]     = $field[ help_text ];
								$lang->update_values ( $lang->data );
								$lang->save ();
						}
						admin_output ( $language[ edit_lang_title ] , "<br /><b>$CGI[new_language] Created</b><br />You will need to reload the language manager to see it" );
				}


				// --------------------------------------------------------------------- //
				//  Import Language Set from Support Server
				// --------------------------------------------------------------------- //
				function import_language ()
				{
						global $CGI;
						global $iSQL;
						$saved = '';
						if ( $CGI[ module ] ) {
								$CGI[ section ] = $CGI[ module ];
						}
						foreach ( $CGI as $field => $value ) {
								if ( $field != "section" && $field != "module" && $field != "language" && $field != "new_name" && $field != "new_value" && $field != "action" ) {
										$lang                        = new Record( array ( "Parent" => $iSQL , "Table" => LANGUAGE_TABLE , "Schema" => LANGUAGE_TABLE , "Lookup" => 1 , "language" => $CGI[ language ] , "section" => $CGI[ section ] , "setting_name" => $field ) );
										$lang->data[ language ]      = $CGI[ language ];
										$lang->data[ section ]       = $CGI[ section ];
										$lang->data[ setting_name ]  = $field;
										$lang->data[ current_value ] = $CGI[ $field ];
										$lang->update_values ( $lang->data );
										$lang->save ();
										$saved .= "$CGI[language]/$CGI[section]/$field: $CGI[$field] $language[language_saved]<br /><br />\n";
								}
						}
						admin_output ( $language[ edit_lang_title ] , "<br /><b>$CGI[language] / $CGI[module]</b><br />$saved" );
				}


				// --------------------------------------------------------------------- //
				//  HTML Substitutions: List
				// --------------------------------------------------------------------- //
				function html_substitutions ()
				{
						global $oSQL;
						global $language;
						global $SCRIPT_NAME;
						$form         = '';
						$SQL          = "SELECT * FROM html_replacements";
						$replacements = $oSQL->search ( array ( "SQL" => $SQL ) );
						$x            = 0;
						foreach ( $replacements as $replacement ) {
								$x++;
								$replacement[ source_tag ] = preg_replace ( "/\'/" , '"' , $replacement[ source_tag ] );
								$replacement[ dest_tag ]   = preg_replace ( "/\'/" , '"' , $replacement[ dest_tag ] );
								$form .= "
                   <tr>
                      <td valign='middle'><input name='source[$x]' value='$replacement[source_tag]' size=5></td>
                      <td valign='middle'><textarea name='dest[$x]' rows='1' cols='50'>$replacement[dest_tag]</textarea></td>
                   </tr>
               ";
						}
						$output = "
              <form action='$SCRIPT_NAME' method='post'>
                 <table width=500>

                   $form

                   <tr>
                      <td colspan=2>
                        New:
                        <input name='new_source' size=5>
                        <input name='new_dest'>
                      </td>
                   </tr>

                   <tr>
                       <td colspan=2 align='center'>
                          <input type='hidden' name='action' value='save_substitutions'>
                          <input type='submit' value='$language[html_substitution_save_button]'>
                       </td>
                   </tr>
                 </table>
              </form>
           ";
						admin_output ( $language[ html_substitution_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  HTML Substitutions: Save
				// --------------------------------------------------------------------- //
				function save_substitutions ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $data_dir;
						$out    = '';
						$source = $CGI[ source ];
						$dest   = $CGI[ dest ];
						foreach ( $source as $num => $value ) {
								$src      = $source[ $num ];
								$dst      = $dest[ $num ];
								$show_src = htmlentities ( $src );
								$show_dst = htmlentities ( $dst );
								$out .= "<tr><td nowrap='nowrap'>$show_src</td><td nowrap='nowrap'><b>replaced by:</b></td><td>$show_dst</td></tr> ";
								$word                     = new Record( array ( "Parent" => $iSQL , "Table" => 'html_replacements' , "Schema" => 'html_replacements' , "Lookup" => 1 , "source_tag" => $src ) );
								$word->data[ source_tag ] = $src;
								$word->data[ dest_tag ]   = $dst;
								$word->update_values ( $word->data );
								// $word->save();
						}
						if ( $CGI[ new_source ] ) {
								$out .= " <tr><td nowrap='nowrap'><i>$CGI[new_source]</td><td nowrap='nowrap'><b>replaced by:</td><td>$CGI[new_dest]</td></tr> ";
								$word                     = new Record( array ( "Parent" => $iSQL , "Table" => 'html_replacements' , "Schema" => 'html_replacements' , "Lookup" => 1 , "source_tag" => $CGI[ new_source ] ) );
								$word->data[ source_tag ] = $CGI[ new_source ];
								$word->data[ dest_tag ]   = $CGI[ new_dest ];
								$word->update_values ( $word->data );
								$word->save ();
						}
						$output = "<table border='1' cellpadding='4'>$out</table>";
						admin_output ( $language[ html_substitution_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  Glossary Search
				// --------------------------------------------------------------------- //
				function glossary_search ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $SCRIPT_NAME;
						$word               = new Record( array ( "Parent" => $oSQL , "Table" => GLOSSARY_TABLE , "Schema" => GLOSSARY_TABLE , "Lookup" => 1 , "word" => $CGI[ word ] ) );
						$word->data[ word ] = $CGI[ word ];
						$formarray          = $word->generate_form ();
						$form               = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$form .= "<tr><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$output = "
              <br />
              <form action='$SCRIPT_NAME' method='post'>
              <table border='0' width='500' cellpadding='5' cellspacing='0'>
                <tr bgcolor='navy'>
                   <td colspan=2>
                     <font color='white'>$language[glossary_info] <b>$CGI[word]</b></font>
                   </td>
                <tr>
                $form
                <tr><td colspan=2 align='center'>
                    <input type='hidden' name='action' value='glossary_save'>
                    <input type='submit' value='$language[glossary_save_button]'>
                </td></tr>
              </table>
              </form>
           ";
						admin_output ( $language[ glossary_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  Glossary Save
				// --------------------------------------------------------------------- //
				function glossary_save ()
				{
						global $iSQL;
						global $CGI;
						$word = new Record( array ( "Parent" => $iSQL , "Table" => GLOSSARY_TABLE , "Schema" => GLOSSARY_TABLE , "Lookup" => 1 , "word" => $CGI[ word ] ) );
						$word->update_values ( $CGI );
						$word->save ();
						admin_output ( $language[ glossary_title ] , "<br /><b>$CGI[word]</b> Saved.<br />" );
				}



				// --------------------------------------------------------------------- //
				//  Lexicals Search
				// --------------------------------------------------------------------- //
				function lexical_search ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $SCRIPT_NAME;
						$word    = new Record( array ( "Parent" => $oSQL , "Table" => LEXICAL_TABLE , "Schema" => LEXICAL_TABLE ) );
						$list    = '';
						$SQL     = "SELECT synonym FROM " . LEXICAL_TABLE . " WHERE word = '$CGI[word]'";
						$synonym = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
						if ( $synonym ) {
								$SQL   = "SELECT word FROM " . LEXICAL_TABLE . " WHERE synonym = '$synonym' ORDER BY word";
								$words = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
								$list  = join ( "\n" , $words );
						}
						$word->data[ word ]    = $CGI[ word ];
						$word->data[ synonym ] = $list;
						$formarray             = $word->generate_form ();
						$form                  = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$form .= "<tr><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$output = "
              <br />
              <form action='$SCRIPT_NAME' method='post'>
              <table border=0 width=500 cellpadding=5 cellspacing=0>
                <tr bgcolor='navy'>
                   <td colspan=2>
                     <font color='white'>$language[lexical_info] <b>$CGI[word]</b></font>
                   </td>
                <tr>
                $form
                <tr><td colspan=2 align='center'>
                    <input type='hidden' name='action' value='lexical_save'>
                    <input type='submit' value='$language[lexical_save_button]'>
                </td></tr>
              </table>
              </form>
           ";
						admin_output ( $language[ lexical_title ] , $output );
				}

				// --------------------------------------------------------------------- //
				//  Lexicals Search
				// --------------------------------------------------------------------- //
				function lexical_save ()
				{
						global $CGI;
						global $language;
						global $iSQL;
						$terms    = array ();
						$report   = '';
						$synlist  = preg_replace ( "/(\r|\n|\cM)/" , "===" , $CGI[ synonym ] );
						$synonyms = split ( '===' , $synlist );
						foreach ( $synonyms as $synonym ) {
								$synonym           = preg_replace ( "/\n|\r|\cM/" , "" , $synonym );
								$terms[ $synonym ] = 1;
						}
						$terms[ $CGI[ word ] ] = 1;
						## Get the ID from the table ##
						$SQL = "SELECT synonym FROM " . LEXICAL_TABLE . " WHERE word = '$CGI[word]'";
						$id  = $iSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
						if ( !$id ) {
								$SQL = "SELECT max(synonym) FROM " . LEXICAL_TABLE;
								$id  = $iSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
								$id++;
						}
						if ( !$id ) {
								$id = 1;
						}
						## Delete current Assignments
						$SQL = "DELETE FROM " . LEXICAL_TABLE . " WHERE synonym = '$id'";
						$iSQL->run ( $SQL );
						foreach ( $terms as $term => $exists ) {
								if ( !$term ) {
										continue;
								}
								$word                  = new Record( array ( "Parent" => $iSQL , "Table" => LEXICAL_TABLE , "Schema" => LEXICAL_TABLE , "Lookup" => 1 , "word" => $term ) );
								$word->data[ word ]    = $term;
								$word->data[ synonym ] = $id;
								$word->update_values ( $word->data );
								$word->save ();
								$report .= " <tr><td>$term</td><td>&lt;=&gt;</td><td>$id</td></tr> ";
						}
						admin_output ( $language[ lexical_title ] , "<table>$report</table>" );
				}



				// --------------------------------------------------------------------- //
				//  Custom Plugin Editor
				// --------------------------------------------------------------------- //
				function plugins ( $message = '' )
				{
						global $SCRIPT_NAME;
						global $CGI;
						global $data_dir;
						global $language;
						$file    = '';
						$plugin  = '';
						$plugins = array ();
						if ( $handle = opendir ( "$data_dir/custom" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( preg_match ( "/\.inc/i" , $file ) ) {
												$plugin_name             = preg_replace ( "/\.inc/i" , "" , $file ) . " (PHP)";
												$plugins[ $plugin_name ] = $file;
										}
								}
								closedir ( $handle );
						}
						$plist = '';
						ksort ( $plugins );
						foreach ( $plugins as $plugin => $file ) {
								$plist .= "<option value='$file'>$plugin</option>";
						}
						$output = "
              $message
              <font size=+2><b><i>Custom Plugin Manager</i></b></font>
              <table border=0 bgcolor='#e0e0e0' cellspacing=3 cellpadding=5 width=100%>
                  <font face='verdana,arial, helvetica' size='2' color='#000000'>
                  <form action='$SCRIPT_NAME' method='post'>
                  $language[select_plugin]
                  <select name='plugin'>
                  $plist
                  </select>
                  <br /><br />
                  $language[create_plugin] <input name='new_plugin'>
                  <br /><br />
                  <input name='action' value='edit_plugin' type='hidden'>
                  <center><input value='$language[edit_plugin_button]' type='submit'></center>

                  </form>
                  </font>
              </table>
           ";
						admin_output ( $language[ plugins_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  Edit a custom plugin
				// --------------------------------------------------------------------- //
				function edit_plugin ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $data_dir;
						global $language;
						$code       = '';
						$plugin     = $CGI[ plugin ];
						$new_plugin = $CGI[ new_plugin ];
						if ( $new_plugin ) {
								$plugin      = cleanup_string ( "$new_plugin" );
								$plugin_name = preg_replace ( "/\.inc/" , "" , $plugin );
								$code        = "<?php\n\n   ### Plugin: $plugin_name\n   ### Purpose:\n\n   function $plugin_name() {\n\n   }\n\n?>";
								$plugin .= ".inc";
						} else {
								$code = implode ( "" , file ( "$data_dir/custom/$plugin" ) );
						}
						$plugin_name = preg_replace ( "/\.inc/" , "" , $plugin );
						$output      = "
              <table border=0 cellspacing=3 cellpadding=5>
               <tr><td>
                  <form action='$SCRIPT_NAME' method='post'>
                  <textarea style='width:95%;font-family:courier; font-size:8pt' name='code' cols='60' rows='20'>$code</textarea>
                  <br /><br />
                  <center>
                  <input type='hidden' name='plugin' value='$plugin'>
                  <input type='hidden' name='action' value='save_plugin'>
                  <input type='submit' value='$language[save_plugin_button]'>
                  <b><a href='http://www.iwebsupport.com/help/hyperseek/customplugins.htm' target='new'>$language[plugin_help_link]</a></b><br /><br />
                  </center>
                  </form>
               </td></tr>
              </table>
           ";
						admin_output ( "$language[edit_plugin_title] &lt;!-- PLUGIN:CUSTOM:$plugin_name --&gt;" , $output );
				}


				// --------------------------------------------------------------------- //
				//  Save a custom plugin
				// --------------------------------------------------------------------- //
				function save_plugin ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $data_dir;
						global $language;
						$debug       = '';
						$pluginfile  = "$data_dir/custom/$CGI[plugin]";
						$CGI[ code ] = preg_replace ( "/\cM/" , "" , $CGI[ code ] );
						// Let's make sure the file exists and is writable first.
						list ( $success , $message ) = write_file ( $pluginfile , $CGI[ code ] );
						if ( !$success ) {
								$this->plugins ( $message );
								exit;
						}
						// Syntax Check
						system ( "php -l $pluginfile > $data_dir/custom/debug.txt 2>&1" );
						$debug   = implode ( "" , file ( "$data_dir/custom/debug.txt" ) );
						$message = "
                 <div style='padding:5px; border:1px solid black; width:400'>
                 $CGI[plugin] $language[plugin_saved]<br /><br />
                 <b><pre>$debug</pre></b>
                 </div>
           ";
						$this->plugins ( $message );
				}


				// --------------------------------------------------------------------- //
				//  Check for upgrades
				// --------------------------------------------------------------------- //
				function versions ( $return = 0 )
				{
						global $config;
						global $LICENSE_TYPE;
						$version         = .99;
						$upgrade_message = '';
						$versioncolor    = "black";
						$license_no      = $config[ license_number ];
						$latest_version  = implode ( "" , file ( "http://www.hyperseek.com/applications/versions.php?lc=$license_no" ) );
						$info            = file ( "./VERSION" );
						foreach ( $info as $line ) {
								if ( preg_match ( "/VERSION\:(.*)\n/" , $line , $matches ) ) {
										$version = $matches[ 1 ];
										break;
								}
						}
						$version        = preg_replace ( "/(\r|\n|\cM)/" , "" , $version );
						$version        = preg_replace ( "/(^\s+|\s+$)/" , "" , $version );
						$latest_version = preg_replace ( "/(^\s+|\s+$)/" , "" , $latest_version );
						$latest_version = preg_replace ( "/(^\s+|\s+$)/" , "" , $latest_version );
						if ( preg_match ( "/invalid/i" , $latest_version ) ) {
								$upgrade_message = "Invalid Registration: $license_no";
								$latest_version  = "n/a";
								$versioncolor    = "green";
						} elseif ( $version < $latest_version ) {
								$upgrade_message = "
                   <a href='http://www.iwebsupport.com/' target='new'>Upgrade available</a>
               ";
								$versioncolor    = "red";
						} elseif ( $version == $latest_version ) {
								$upgrade_message = "Your version is current";
								$versioncolor    = "blue";
						} else {
								$upgrade_message = "Possible mismatch in versions";
								$versioncolor    = "green";
						}
						$ulink = '';
						if ( !preg_match ( "/unlimited|source/i" , $LICENSE_TYPE ) ) {
								$ulink = "(<a href='http://www.hyperseek.com/index.php/purchase/' target='_blank'>Upgrade</a>)";
						}
						$output = "
              <table border='0' cellspacing='0' cellpadding='5'>
               <tr>
                  <td nowrap valign='top'>
                      <b>License Type:</b>
                  </td>
                  <td nowrap valign='top' align='right'>
                     $LICENSE_TYPE
                     $ulink
                  </td>
               </tr>
               <tr>
                  <td nowrap valign='top'> <b>Your Version: </b> </td>
                  <td nowrap valign='top' align='right' style='color:$versioncolor'>$version</td>
               </tr>
               <tr>
                  <td nowrap valign='top'> <b>Latest Version: </b> </td>
                  <td nowrap valign='top' align='right' style='color:blue'>$latest_version</td>
               </tr>
               <tr>
                  <td colspan='2' align='center'>
                      $upgrade_message
                  </td>
               </tr>
              </table>
           ";
						if ( $return ) {
								return $output;
						}
						admin_output ( $language[ check_version_title ] , $output );
				}



				// --------------------------------------------------------------------- //
				//  Re-Set Important Unix Permissions
				// --------------------------------------------------------------------- //
				function permissions ()
				{
						global $CGI;
						global $data_dir;
						global $SCRIPT_NAME;
						global $language;
						if ( $CGI[ confirm ] == 1 ) {
								system ( "chmod -R 755 *" );
								system ( "chmod -R 777 $data_dir" );
								$output = "
                  $language[permissions_programs] (755)<br />
                  $language[permissions_data] (777)<br />
               ";
								admin_output ( $language[ permissions_title ] , $output );
						} else {
								$output = "
                  <table border=0 width=400>
                      <tr>
                         <td>
                            $language[confirm_permissions]
                            <br /><br />
                            <a href='$SCRIPT_NAME?action=permissions&confirm=1'>confirm</a>
                         </td>
                      </tr>
                  </table>
               ";
								admin_output ( $language[ permissions_title ] , $output );
						}
				}

				// --------------------------------------------------------------------- //
				//  Backup
				// --------------------------------------------------------------------- //
				function backup ()
				{
						global $CGI;
						global $data_dir;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						if ( $CGI[ confirm ] != 1 ) {
								$output = "
                  <table border=0 width=400>
                      <tr>
                         <td>
                            $language[confirm_backup]
                            <br /><br />
                            <a href='$SCRIPT_NAME?action=backup&confirm=1'>Confirm</a>
                         </td>
                      </tr>
                  </table>
               ";
								admin_output ( $language[ backup_title ] , $output );
						} else {
								## Dump the database
								$sql_backup_file = $data_dir . "/sqlbackup.sql";
								$mysqlcommand    = "mysqldump -u " . $config[ dbusername ] . " -p" . $config[ dbpassword ] . " -h " . $config[ dbhost ] . " " . $config[ dbname ] . "> $sql_backup_file";
								system ( $mysqlcommand );
								$SQL_SIZE = filesize ( $sql_backup_file );
								## Out with the old ...
								unlink ( "$data_dir/iweb_data.tar.gz" );
								## In with the new ...
								$command1 = "cd $data_dir; tar cfz iweb_data.tar.gz *";
								system ( $command1 );
								$DAT_SIZE = filesize ( "$data_dir/iweb_data.tar.gz" );
								if ( $DAT_SIZE > 1 ) {
										$output = "
                      <h2>$language[backup_complete]</h2>
                      <br /><br />
                      $mysqlcommand ...
                          <div style='padding-left:20px'>$language[backup_size] $SQL_SIZE $language[backup_bytes]</div><br />
                      $command1 ...
                          <div style='padding-left:20px'>$language[backup_size] $DAT_SIZE $language[backup_bytes]</div><br />
                      $language[backup_message]
                    ";
								} else {
										$output = "
                      <font color='red'><h1>$language[backup_error]</h1></font>
                      <h5>$language[backup_error_message] $!</h5>
                    ";
								}
								admin_output ( $language[ backup_title ] , $output );
						}
				}



				// --------------------------------------------------------------------- //
				//  Edit the "Notes to myself" area
				// --------------------------------------------------------------------- //
				function edit_notes ()
				{
						global $data_dir;
						global $language;
						global $SCRIPT_NAME;
						$notes  = implode ( "" , file ( "$data_dir/notes.txt" ) );
						$output = "
             <form action='$SCRIPT_NAME' method='post'>
               <table cellspacing=10>
                 <tr>
                   <td valign='top'>
                       <b>$language[edit_notes_title]</b><br />
                       <textarea name='notes' cols=60 rows=8>$notes</textarea><br />
                       <input type='hidden' name='action' value='save_notes'>
                       <center><input type='submit' value='$language[save_notes_button]'></center>
                   </td>
                 </tr>
               </table>
             </form>
          ";
						admin_output ( $language[ edit_notes_title ] , $output );
				}

				// --------------------------------------------------------------------- //
				//  Save the "Notes to myself" area
				// --------------------------------------------------------------------- //
				function save_notes ()
				{
						global $data_dir;
						global $language;
						global $CGI;
						$message = "";
						$file    = "$data_dir/notes.txt";
						list ( $success , $message ) = write_file ( $file , $CGI[ notes ] );
						$this->main_frame ();
				}


				// --------------------------------------------------------------------- //
				//  Load up the iwebsupport.com page properly (don't break the frames)
				// --------------------------------------------------------------------- //
				function support ()
				{
						$output = "
               <script language='javascript'>
                  document.location.href='http://www.iwebsupport.com/';
               </script>
          ";
						admin_output ( "iWeb Application Support" , $output );
				}



				// --------------------------------------------------------------------- //
				//  Template Manager
				// --------------------------------------------------------------------- //
				function templates ()
				{
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $CGI;
						global $oSQL;
						global $data_dir;
						global $include_dir;
						global $vars;
						global $page_templates;
						global $PROGRAMS;
						// List of available Themes
						$themes = "";
						if ( $handle = opendir ( "$data_dir/templates/" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( $file == "." || $file == ".." ) {
												continue;
										}
										$themes .= "
                   <option value='$file'>$file</option>
                ";
								}
								closedir ( $handle );
						}
						// List of page templates
						$userdefined      = '';
						$system_templates = '';
						$system           = array ();
						$user             = array ();
						$applications     = array ();
						// Pre-Defined (system) tempaltes
						$dir = $include_dir . "/dsX/Admin/config/";
						if ( is_dir ( $dir ) ) {
								$PROGRAMS = array ();
								if ( $handle = opendir ( $dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$file              = preg_replace ( "/\.inc/i" , "" , $file );
														$PROGRAMS[ $file ] = 1;
														include_once ( "dsX/Admin/config/$file.inc" );
												}
										}
										closedir ( $handle );
								}
						}
						// This will catch any custom or added on admin settings areas
						$custom_admin_dir = $data_dir . "/admin/";
						if ( is_dir ( $custom_admin_dir ) ) {
								if ( $handle = opendir ( $custom_admin_dir ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( preg_match ( "/\.inc/i" , $file ) ) {
														$appname              = preg_replace ( "/\.inc/" , "" , $file );
														$PROGRAMS[ $appname ] = 1;
														include_once ( "$data_dir/admin/$appname.inc" );
												}
										}
										closedir ( $handle );
								}
						}
						foreach ( $PROGRAMS as $program => $x ) {
								// Clear out the "templates" and "vars" array and reload
								$templates = array ();
								$vars      = array ();
								$function  = "config_" . $program;
								call_user_func ( $function );
								if ( $vars[ 'templates' ] ) {
										$applications[ $program ] = $program;
								}
								$tlist = explode ( "|" , $page_templates[ 'pages' ] );
								if ( is_array ( $tlist ) ) {
										foreach ( $tlist as $template ) {
												$system[ $template ] = $template;
										}
								}
						}
						// System Templates
						ksort ( $system );
						foreach ( $system as $template => $name ) {
								$name = preg_replace ( "/\.txt/" , "" , $name );
								$system_templates .= " <option value='$name'>$name</option> ";
						}
						// User Defined Page Templates
						if ( $handle = opendir ( "$data_dir/templates/default/" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( !preg_match ( "/\.txt/i" , $file ) ) {
												continue;
										}
										$user[ $file ] = $file;
								}
								closedir ( $handle );
								ksort ( $user );
								foreach ( $user as $template => $name ) {
										$name = preg_replace ( "/\.txt/" , "" , $name );
										if ( $system[ $name ] ) {
												continue;
										}
										$userdefined .= " <option value='$name'>$name</option> ";
								}
						}
						$pages = "
             $system_templates
             <option value=''>---------------------
             <option value=''>$language[page_templates_user_defined]
             $userdefined
          ";
						// Generate the applications list
						$alist = array ();
						foreach ( $applications as $app => $name ) {
								$program = preg_replace ( "/\.inc/" , "" , $app );
								$appname = $language[ $program ];
								if ( !$appname ) {
										$appname = $program;
								}
								$alist[ $appname ] = $program;
						}
						$application_list = '';
						ksort ( $alist );
						foreach ( $alist as $name => $program ) {
								$application_list .= "
                 <option value='$program'>$name</option>
              ";
						}
						## The titles for the admin tabs, in the order that they'll appear accross
						## the top of the tabset.
						$tab_titles = array ( $language[ tab_pages ] , $language[ tab_internal ] , $language[ tab_themes ] );
						## The tab contents for each of the above titles, note that
						## the titles and hash keys are the SAME.
						$tab_definition = array ( $language[ tab_pages ] => "
                           <form action='$SCRIPT_NAME' target='frame-T1'>
                                $language[select_theme_prompt] <select name='theme'>$themes</select><br>
                                $language[select_template_prompt] <select name='filename'>$pages</select><br>
                                <center>
                                <input type='submit' value='$language[edit_template_button]'>

                                </center>
                                <input type='hidden' name='action' value='edit_file'>
                           </form>
                       " , $language[ tab_internal ] => "
                           <form action='$SCRIPT_NAME' target='frame-T2'>
                                $language[select_theme_prompt] <select name='theme'>$themes</select><br>
                                $language[select_app_prompt] <select name='application'>$application_list</select><br>
                                <center><input type='submit' value='$language[edit_internal_button]'></center>
                                <input type='hidden' name='action' value='edit_internal_template'>
                           </form>
                       " , $language[ tab_themes ] => "
                           <form style='margin:0px; padding:0px' action='$SCRIPT_NAME' target='frame-T3'>
                                $language[create_theme_prompt]
                                <input type='text' name='new_theme' size='10'> ... $language[copy_theme_from]
                                   <select name='theme'>
                                      $themes
                                      <option value=''>start from scratch</option>
                                   </select>
                                <input type='submit' value='$language[copy_theme_button]'>
                                <input type='hidden' name='action' value='copy_theme'>
                           </form>
                           <form style='margin:0px; padding:0px' action='$SCRIPT_NAME'>
                                $language[delete_theme_prompt]
                                   <select name='theme'>
                                      $themes
                                   </select>
                                <input type='submit' value='$language[delete_theme_button]'>
                                <input type='hidden' name='action' value='delete_theme'>
                           </form>
                       " , );
						$tabs           = $this->admin_tabs ( $tab_titles , $tab_definition );
						admin_output ( "Templates" , $tabs );
				}



				// --------------------------------------------------------------------- //
				//  Edit a page template
				// --------------------------------------------------------------------- //
				function copy_theme ()
				{
						global $data_dir;
						global $CGI;
						global $config;
						global $language;
						global $iSQL;
						if ( $CGI[ new_theme ] ) {
								$theme = cleanup_string ( $CGI[ new_theme ] );
								mkdir ( "$data_dir/templates/$theme" );
								$message = "New Theme: $theme Created.<BR>";
								## Making a copy ?
								if ( $CGI[ theme ] ) {
										## Copy the page templates over.
										if ( $handle = opendir ( "$data_dir/templates/$CGI[theme]/" ) ) {
												while ( false !== ( $file = readdir ( $handle ) ) ) {
														if ( $file == '.' || $file == '..' ) {
																continue;
														}
														copy ( "$data_dir/templates/$CGI[theme]/$file" , "$data_dir/templates/$theme/$file" );
														$message .= "Copying page template: $file from $CGI[theme]<BR>";
												}
												closedir ( $handle );
										}
										## Copy the internal templates over.
										$SQL       = "SELECT * FROM " . TEMPLATES_TABLE . " WHERE theme = '$CGI[theme]'";
										$templates = $iSQL->search ( array ( "SQL" => $SQL ) );
										foreach ( $templates as $template ) {
												$new_template                        = new Record( array ( "Parent" => $iSQL , "Table" => TEMPLATES_TABLE , "Schema" => TEMPLATES_TABLE ) );
												$new_template->data[ theme ]         = $theme;
												$new_template->data[ program ]       = $template[ program ];
												$new_template->data[ template_type ] = $template[ template_type ];
												$new_template->data[ template_name ] = $template[ template_name ];
												$new_template->data[ template_text ] = $template[ template_text ];
												if ( $new_template->update_values ( $new_template->data ) ) {
														if ( $new_template->save () ) {
																$message .= "Copied internal/detail template: $template[program]/$template[template_name] from $CGI[theme]<br />";
														} else {
																$message .= "Error copying internal/detail template: $template[program]/$template[template_name] from $CGI[theme] ( $new_template->error_string )<br />";
														}
												} else {
														$message .= "Error copying internal/detail template: $template[program]/$template[template_name] from $CGI[theme] ( $new_template->error_string )<br /";
												}
										}
								}
						} else {
								$message = "No Theme Name Entered, please retry";
						}
						admin_output ( "Theme" , $message );
				}


				// --------------------------------------------------------------------- //
				//  Delete a theme
				// --------------------------------------------------------------------- //
				function delete_theme ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $data_dir;
						$message = '';
						$theme   = $CGI[ theme ];
						if ( $CGI[ theme ] == "default" ) {
								$message = "Deleting the default theme is not permitted";
						} else {
								unlink ( "$data_dir/config/$theme.inc" ) || die ( "Couldn't remove config file ($data_dir/config/$theme.inc) from this theme, please check it's permissions" );
								$message = "Config File Removed<BR>";
								if ( $handle = opendir ( "$data_dir/templates/$theme/" ) ) {
										while ( false !== ( $file = readdir ( $handle ) ) ) {
												if ( $file == '.' || $file == '..' ) {
														continue;
												}
												unlink ( "$data_dir/templates/$theme/$file" ) || die ( "Couldn't remove $file from this theme, please check it's permissions" );
												$message .= "Deleting page template: $file from $theme<BR>";
										}
										closedir ( $handle );
								}
								rmdir ( "$data_dir/templates/$theme" ) || die ( "Couldn't delete theme directory, please check it's permissions" );
								## Delete Internal Templates
								$SQL = "DELETE FROM " . TEMPLATES_TABLE . " WHERE theme = '$theme'";
								$iSQL->run ( $SQL );
								$message .= "<br><b>Deleted the internal templates for $theme</b><br>";
						}
						admin_output ( "Delete Theme" , $message );
				}


				// --------------------------------------------------------------------- //
				//  Edit a page template
				// --------------------------------------------------------------------- //
				function edit_file ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $data_dir;
						global $config;
						$theme = $CGI[ theme ];
						if ( !$theme ) {
								$theme = 'default';
						}
						$filename     = cleanup_string ( $CGI[ filename ] , "-" , 1 );
						$file         = $data_dir . "/templates/" . $theme . "/" . $filename . ".txt";
						$default_file = $data_dir . "/templates/" . $theme . "/" . "default.txt";
						if ( file_exists ( $file ) ) {
								$html      = implode ( "" , file ( $file ) );
								$read_from = $file;
						} elseif ( file_exists ( $default_file ) ) {
								$html      = implode ( "" , file ( $default_file ) );
								$read_from = $default_file;
						}
						$output = "
                <font face='arial,helvetica' size=-1>
                <br>
                <center>
                <form name='html_form' action='$SCRIPT_NAME' method='post'>
                <table bgcolor='black' border='0' width='630' cellspacing='2' cellpadding='0'>
                    <tr>
                      <td>
                          <table width='100%' border='0' bgcolor='#e0e0e0' cellspacing='0'>
                            <tr>
                              <td valign='top' bgcolor='#cccc99'>
                                <font face='arial,helvetica' size='-1'>
                                <h3>template editor</h3>
                                <b>filename: $filename</b>
                                <br>(read in from: $read_from)
                                </font>
                              </td>
                              <td valign='top' bgcolor='#cccc99' align='right'>
                                <a href='$config[html_dir]/dsx/hyperseek/html/thetemplateeditor.htm' target='new'><img src='$config[button_dir]/help-icon.gif' border=0></a>
                              </td>
                            </tr>
                              <td valign='top' colspan=2>
                               <center>
                               <textarea name='source' cols=70 rows=25>$html</textarea>
                                  <br>
                                  <input type='hidden' name='action' value='save_file'>
                                  <input type='hidden' name='id' value='$CGI[id]'>
                                  <input type='hidden' name='theme' value='$CGI[theme]'>
                                  <center>
                                    Template: <input name='filename' value='$CGI[filename]'>
                                    <input type='submit' value='Save File'>
                                  </center>
                               </center>
                              </td></tr>
                            </table>
                      </td>
                   </tr>
                </table>
                </form>
              </center>
          ";
						echo $output;
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Save a page template
				// --------------------------------------------------------------------- //
				function save_file ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $data_dir;
						global $config;
						$theme = $CGI[ theme ];
						if ( !$theme ) {
								$theme = "default";
						}
						$filename = cleanup_string ( $CGI[ filename ] , "-" , 1 );
						$file     = $data_dir . "/templates/" . $theme . "/" . $filename . ".txt";
						$TEXT     = $CGI[ source ];
						$TEXT     = preg_replace ( "/\cM/" , "" , $TEXT );
						$message  = '';
						if ( !( $TEXT ) ) {
								unlink ( $file );
								$act = "Deleted";
						} else {
								list ( $status , $act ) = write_file ( $file , $TEXT );
						}
						if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
								unlink ( "$data_dir/phpsettings.inc" );
						}
						// For Display ...
						$TEXT = preg_replace ( "/<!--/" , "&lt;!--" , $TEXT );
						$TEXT = preg_replace ( "/-->/" , "--&gt;" , $TEXT );
						echo "
              <html>
                 <body>
                      <br><br>
                      <center><h3>File ($file) $act.</h3>
                      (Comments/Plugins shown as text below)
                      <form><input type='button' value='--- OK ---' onclick='history.go(-1);'></form>
                      <hr width=80%>
                      </center>
                      $TEXT
                 </body>
               </html>
          ";
						exit;
				}



				// --------------------------------------------------------------------- //
				//  Edit an internal template
				// --------------------------------------------------------------------- //
				function edit_internal_template ( $message = '' )
				{
						include_once ( "dsX/SubTemplate.inc" );
						global $language;
						global $config;
						global $data_dir;
						global $CGI;
						global $SCRIPT_NAME;
						global $iSQL;
						global $vars;
						global $include_dir;
						$n         = '\r\n';
						$last_type = '';
						$theme     = $CGI[ theme ];
						$program   = $CGI[ application ];
						## Get some dummy data ...
						$dummydata         = array ( 'main_category' => "<A HREF='/search/'>Dummy Category</A>" , 'category' => "<A HREF='/search/'>Dummy Category</A>" , 'subs' => "<A HREF='/search/'>A Sub Category</A>" , 'folder' => "<IMG SRC=$global{button_dir}/folder.gif>" , 'catfolder' => "<IMG SRC=$global{button_dir}/big_folder.gif>" , 'catlink' => "<A HREF='/search/'>A Sub Category</A>" , 'count' => 15 , 'desc' => "A Category Description" , 'referer' => "<A HREF='javascript:history.go(-1)'>Back</A>" , 'parent' => "<A HREF='/search/'>Dummy Category</A>" , 'current' => "<A HREF='/search/'>Another Sub Category</A>" , 'question' => "How are you" , 'input' => "<SELECT><OPTION>Yes<OPTION>No</SELECT>" , 'percentage' => "65%" , 'answer' => "Yes" , 'graph' => "<IMG SRC=$global{button_dir}/iseekbar.gif height=5 width=75>" , 'total' => 150 , 'new' => "<IMG SRC=$global{button_dir}/new.gif>" , 'cool' => "<IMG SRC=$global{button_dir}/cool.gif>" , 'rating' => "[<A HREF='dsx.cgi?app=Rateit&id=12345'>Rating</A>: 8.5, 4 Votes]" , 'recommend' => "dsx.cgi?app=SE&action=recommend&id=12345" , 'dead' => "dsx.cgi?app=SE&action=dead&id=12345" , 'cost' => "Cost to advertiser: \$.05" , 'xfactor' => "9.5" , 'title_tag' => "2" , 'last_status' => "ACTIVE" , 'account' => "unassigned" , 'phone' => "555-1212" , 'site_icon' => "3" , 'category' => "Software:Programs" , 'title' => "Jet City Software" , 'fax' => "555-1212" , 'city' => "Sometown" , 'top_votes' => "0" , 'url' => "http://www.testing.com" , 'zip' => "99999" , 'rate_score' => "0" , 'id' => "991963625" , 'rate_votes' => "0" , 'email' => "me\@here.com" , 'rank' => "5.00000" , 'startdate' => "" , 'description' => "Welcome to my wonder test website" , 'country' => "USA" , 'enddate' => "07/06/2001" , 'hits' => "10" , 'name' => "Fred Smith" , 'av_links' => "0" , 'qualifier' => "English" , 'keywords' => "  testing, test, website   " , 'price' => ".15" , 'state' => "WA" , 'desc_image' => "1" , 'address' => "123 Main St" , 'extra_link' => "" , 'msn_id' => "me\@here.com" , 'banned_flag' => "NO" , 'phone' => "555-1212" , 'last_pd_date' => "" , 'num_posts' => "33" , 'contact_name' => "Fred Smith" , 'current_balance' => "25.40" , 'last_ip' => "111.111.111.111" , 'acct_id' => "fsmith" , 'theme' => "default" , 'comments' => "Hello World!" , 'acct_password' => "mypassword" , 'acct_type' => "Webmaster" , 'aol_id' => "myAOL" , 'icq_id' => "11111111" , 'last_visit' => "994188816" , 'signature' => "I am very cool" , 'total_paid' => "0.00" , 'subscribed' => "YES" , 'address' => "g" , 'days_to_show' => "50" , 'contact_email' => "me\@here.com" , 'num_visits' => "21" , );
						$dummydata[ link ] = "<a href='$dummydata[url]'>$dummydata[title]</a> ";
						$SQL               = "SELECT * FROM " . TEMPLATES_TABLE . " WHERE theme='$theme' AND program='$program'";
						$internaltemplates = $iSQL->search ( array ( "SQL" => $SQL ) );
						## Double check that we have all that's required of us.
						$define = array ();
						## Read in the requirements, if any.
						$file        = $include_dir . "/dsX/Admin/config/" . $program . ".inc";
						$custom_file = $data_dir . "/admin/" . $program . ".inc";
						if ( file_exists ( $file ) ) {
								include_once ( $file );
						} elseif ( file_exists ( $custom_file ) ) {
								include_once ( $custom_file );
						}
						## Pre-Defined Templates from DB
						if ( $internaltemplates ) {
								foreach ( $internaltemplates as $template ) {
										$defined[ $template[ template_type ] ][ $template[ template_name ] ] = 1;
								}
						}
						## Required templates from Config File
						$function_name = "config_" . $program;
						if ( function_exists ( $function_name ) ) {
								call_user_func ( $function_name );
						}
						$vt = $vars[ templates ];
						if ( $vt ) {
								foreach ( $vt as $type => $list ) {
										if ( $list ) {
												$tt = split ( "\|" , $list );
												foreach ( $tt as $req ) {
														if ( !$defined[ $type ][ $req ] ) {
																array_push ( $internaltemplates , array ( "template_type" => $type , "template_name" => $req ) );
														}
												}
										}
								}
						}
						aasort ( $internaltemplates , "+template_type,+$template_name" );
						foreach ( $internaltemplates as $template ) {
								$template_code = $template[ template_text ];
								$type          = $template[ template_type ];
								if ( !$last_type ) {
										$last_type = $type;
								}
								if ( $type != $last_type ) {
										$emails .= " <option value=''></option> ";
										$last_type = $type;
								}
								$name  = $template[ template_name ];
								$front = "<<";
								$back  = ">>";
								if ( preg_match ( "/mail/i" , $template[ template_type ] ) ) {
										$front = '\[';
										$back  = '\]';
								}
								$tt     = new SubTemplate( array ( "front" => "$front" , "back" => "$back" , "template_text" => $template_code , "values" => $dummydata ) );
								$sample = $tt->merge ();
								if ( preg_match ( "/searchboxcode|listing|layout/i" , $type ) ) {
										$sample = preg_replace ( "/\n/" , "" , $sample );
								} else {
										$sample = preg_replace ( "/\n/" , "<br />" , $sample );
								}
								$sample        = preg_replace ( "/\r|\cM/" , "" , $sample );
								$sample        = preg_replace ( "/\'/" , "\\'" , $sample );
								$template_code = preg_replace ( "/\n/" , "$n" , $template_code );
								$template_code = preg_replace ( "/\r|\cM/" , "" , $template_code );
								$template_code = preg_replace ( "/\'/" , "\\'" , $template_code );
								## Script Errors Avoided ##
								$template_code = preg_replace ( "/<\/script>/i" , "</scr'+'ipt\>" , $template_code );
								$sample        = preg_replace ( "/<\/script>/i" , "</scr'+'ipt\>" , $sample );
								$js .= "
                    templates['$name'] = '$template_code';
                    ttypes['$name'] = '$type';
                 ";
								$js .= " samples['$name'] = '$sample'; \n ";
								$emails .= " <option value='$name'>&nbsp;&nbsp;&#187;&nbsp;$type: $name</option> ";
						}
						echo "
             <html>
             <body>

              <style>

                 body, td {
                    background:#f5f5f5;
                    font-family:verdana, arial, helvetica;
                    font-size:9pt;
                    color:black;
                 }


                 #divMessage {position:absolute; top:300; left:100;}
              </style>

              <script language='javascript'>

                  function checkBrowser(){
                    this.ver=navigator.appVersion
                    this.dom=document.getElementById?1:0
                    this.ie5=(this.ver.indexOf('MSIE 5')>-1 && this.dom)?1:0;
                    this.ie6=(this.ver.indexOf('MSIE 6')>-1 && this.dom)?1:0;
                    this.ie4=(document.all && !this.dom)?1:0;
                    this.ns5=(this.dom && parseInt(this.ver) >= 5) ?1:0;
                    this.ns4=(document.layers && !this.dom)?1:0;
                    this.bw=( this.ie6 || this.ie5 || this.ie4 || this.ns4 || this.ns5)
                    return this
                  }

                  bw=new checkBrowser();

                  var templates = new Array;
                  var ttypes = new Array;
                  var samples = new Array;
                  $js

                  function editTemplate( form ) {
                     var selected = form.tname.selectedIndex;
                     var template = form.tname.options[selected].value;
                     if ( template ) {
                         var code = templates[template];
                         var type = ttypes[template];
                         form.template_name.value = template;
                         form.template_text.value = code;
                         form.template_type.value = type;
                         changeText(template)
                     }
                  }

                  //Constructing the ChangeText object
                  function makeChangeTextObj(obj){
                     this.writeref=bw.dom? document.getElementById(obj):bw.ie4?document.all[obj]:bw.ns4?document.layers[obj].document:0;
                     this.writeIt=b_writeIt;
                  }

                  function b_writeIt(text,num){
                     if(bw.ns4){
                       this.writeref.open();
                       this.writeref.write(text);
                       this.writeref.close();
                     }
                     else if(bw.dom||bw.ie4) this.writeref.innerHTML=samples[num]
                  }

                  //The mouoseover function. Calls the objects writeIt method to write the text to the div.
                  function changeText(num){
                      if(bw.bw) oMessage.writeIt(samples[num],num)
                  }

                  //The init function. Calls the object constructor and set some properties.
                  function changeTextInit(){
                     if(bw.bw){
                       oMessage=new makeChangeTextObj('divMessage')
                    }
                  }

                  function goHelp() {
                    var Loc = 'http://www.iwebsupport.com/help/dsx/hyperseek/html/internaldetailtemplates.htm';
                    help=open(Loc,'help','toolbar=no,scrollbars=yes,directories=no,menubar=no,resizable=yes,width=500,height=400');
                    help.focus();
                  }

                  //Calls the init function on pageload.
                  onload=changeTextInit

              </script>
              <b>$message</b>
              <table align='left' border='0' cellspacing='10'>
                 <tr>
                    <td valign='top' align='left' colspan=2>
                       <b><a href='javascript:goHelp();'>$language[internal_templates_title]</a></b>
                    </td>
                 </tr>

                <form name='tform' action='$prog_url' method='post'>

                 <tr>
                    <td valign='top'>
                    <b>$language[internal_templates_select]</b><br>
                    <select style='background:#efefef;color:navy;font-size:8pt; font-family: verdana, arial, helvetica' name='tname' size=10 onchange='editTemplate(this.form)'>
                       <option value=''>$language[$program] templates</option>
                       $emails
                    </select>
                    </td>

                    <td valign='top'>
                      $language[new_internal_prompt] <input style='background:#efefef;color:navy;font-size:8pt;font-family: verdana, arial, helvetica' name='new_template'>
                      <select name='new_type'>
                         <option value='EMAIL'>EMAIL</option>
                         <option value='LAYOUT'>LAYOUT</option>
                         <option value='LISTING'>LISTING</option>
                      </select>
                      <br>
                      <B>$language[internal_code_header]</B><BR>

                      <textarea style='background:#e0e0e0;color:maroon;font-size:8pt;' name='template_text' cols=50 rows=8></textarea>

                      <br>

                      <center>
                       <input type='hidden' name='action' value='save_internal_template'>
                       <input type='hidden' name='type' value=''>
                       <input type='hidden' name='template_type' value=''>
                       <input type='hidden' name='theme' value='$theme'>
                       <input type='hidden' name='program' value='$program'>
                       <input type='hidden' name='template_name' value=''>
                       <input type='checkbox' name='taction' value='delete'>delete selected template?
                       <input type='submit' value='$language[save_templates_button]'>
                      </center>
                    </td>
                 </tr>

                 </form>

                 <tr>
                    <td colspan=2>
                      <pre>
                       <div id='divMessage'>
                        $language[email_templates_sample]
                       </div>
                      </pre>
                    </td>
                 </tr>

              </table>

             </body>
             </html>
           ";
						exit;
				}


				// --------------------------------------------------------------------- //
				//  Save an internal template
				// --------------------------------------------------------------------- //
				function save_internal_template ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $data_dir;
						// On a new template, make it look like an existing one...
						if ( $CGI[ new_template ] ) {
								$CGI[ template_name ] = $CGI[ new_template ];
								$CGI[ template_type ] = $CGI[ new_type ];
						}
						$template = new Record( array ( "Parent" => $iSQL , "Table" => TEMPLATES_TABLE , "Schema" => TEMPLATES_TABLE , "Lookup" => 1 , "theme" => $CGI[ theme ] , "program" => $CGI[ program ] , "template_type" => $CGI[ template_type ] , "template_name" => $CGI[ template_name ] ) );
						if ( $CGI[ taction ] == "delete" && $template->delete () ) {
								if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
										unlink ( "$data_dir/phpsettings.inc" );
								}
								$message = "$CGI[template_name] ($CGI[theme] / $CGI[program] / $CGI[template_type])deleted";
						} else {
								if ( $template->update_values ( $CGI ) ) {
										if ( $template->save () ) {
												$message = "<i>$CGI[template_type]: $CGI[template_name]</i> $language[internal_templates_saved]";
										} else {
												$message = "<i>$CGI[template_type]: $CGI[template_name]</i> $language[internal_templates_save_failed] <pre>$template->error_string</pre>";
										}
								} else {
										$message = "<i>$CGI[template_type]: $CGI[template_name]</i> $language[internal_templates_save_failed] <pre>$template->error_string</pre>";
								}
						}
						if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
								unlink ( "$data_dir/phpsettings.inc" );
						}
						$CGI[ application ] = $CGI[ program ];
						$this->edit_internal_template ( $message );
						exit;
				}

				// --------------------------------------------------------------------- //
				//  Helper Functions (Generators)
				// --------------------------------------------------------------------- //
				function get_language_options ()
				{
						global $oSQL;
						$langopts = '';
						$SQL      = "SELECT DISTINCT language FROM " . LANGUAGE_TABLE;
						$list     = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
						foreach ( $list as $language ) {
								$langopts .= " <option value='$language'>$language</option> \n ";
						}

						return $langopts;
				}



				// --------------------------------------------------------------------- //
				//  Apply SQL Patch
				// --------------------------------------------------------------------- //
				function apply_patch ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $data_dir;
						$errors     = '';
						$TEXT       = '';
						$INT        = '';
						$create_log = '';
						$x          = 0;
						$errors     = array ();
						$patch      = $CGI[ patch_name ];
						$file       = "$data_dir/sql/schemas/$patch";
						$sql        = implode ( "" , file ( $file ) );
						$statements = split ( '#STMT#' , $sql );
						foreach ( $statements as $SQL ) {
								if ( !$SQL ) {
										continue;
								}
								$x++;
								if ( $x == 50 ) {
										$x = 0;
										$create_log .= "<br />";
								}
								$iSQL->run ( $SQL , 1 );
								if ( $iSQL->error ) {
										array_push ( $errors , $iSQL->error );
										$create_log .= "<span style='color:red'>x</span>";
								} else {
										$create_log .= "<span style='color:blue'>+</span>";
								}
						}
						rename ( "$data_dir/sql/schemas/$patch" , "$data_dir/sql/schemas/$patch.done" );
						$create_log .= "<hr /><b>Exceptions (<span style='color:red'>red x's above</span>) explained below</b><br /><br />";
						$create_log .= join ( '<br />' , $errors );
						admin_output ( "Patch Applied" , "<div style='text-align:left'>Patch Applied: $data_dir/sql/schemas/$patch<blockquote>$create_log<br /><br /><b>Please Re-Load your admin center.</div>" );
				}

				// --------------------------------------------------------------------- //
				//  Javascript Loaders
				// --------------------------------------------------------------------- //
				function admin_tabs ( $titles = array () , $tabs = array () )
				{
						$height_offset = 200;
						$width_offset  = 300;
						$headers       = '';
						$content       = '';
						$x             = 0;
						foreach ( $titles as $tab ) {
								$x++;
								$tabname     = "tab" . $x . "_head";
								$contentname = "T" . $x;
								$visible     = '';
								if ( $x == 1 ) {
										$visible = "style='visibility: visible'";;
								}
								$style = "style='text-decoration:none'";
								$title = $tab;
								$title = preg_replace ( "/ /" , "&nbsp;" , $title );
								$headers .= "
                 <td id='$tabname' class='tab-button'><a href='#' $style onClick='changeCont(\"$x\", \"$tabname\"); return false;' onFocus='if(this.blur)this.blur()'>$title</a></td>
            ";
								$iframe = '';
								if ( !$hideframe ) {
										$iframe = "<script language='JavaScript'>draw_iframe('frame-$contentname')</script>";
								}
								$content .= "
                <div id='$contentname' class='tab-body' $visible>
                    <table width='100%'>
                       <tr>
                         <td>
                            $tabs[$tab]
                         </td>
                       </tr>
                       <tr>
                         <td>
                            $iframe
                         </td>
                       </tr>
                    </table>
                </div>
            ";
						}

						return "
            <script language='JavaScript'>

                var NS4 = (document.layers) ? 1 : 0;
                var IE = (document.all) ? 1 : 0;
                var DOM = 0;
                if (parseInt(navigator.appVersion) >=5) {DOM=1};

                var max_height = 640;
                var max_width = 600;

                if (parseInt(navigator.appVersion)>3) {
                     max_height = window.innerHeight;
                     max_width = window.innerWidth;

                     if ( IE ) {
                         max_height = parent.document.body.offsetHeight;
                         max_width = parent.document.body.offsetWidth;
                     }
                }

                max_width = max_width - $width_offset;
                max_height = max_height - $height_offset;

                function draw_iframe( fname ) {

                     var height = max_height - 10;
                     var width = max_width - 10;

                     document.write(\"<iframe style='overfill:hidden' name='\" + fname + \"' width='\" + width + \"' height='\" + height + \"'></iframe>\");
                }

                var lastHeader;

                function changeCont(tgt,header) {

                    target=('T' +tgt);

                    if (DOM) {

                        // Hide the last one, and flip the tab color back.
                        currShow.style.visibility='hidden';
                        if ( lastHeader ) { lastHeader.style.background = '#e0e0e0'; lastHeader.style.fontWeight='normal'; }

                        // Show this one, and make the tab silver
                        var flipOn = document.getElementById(target);
                        flipOn.style.visibility='visible';

                        var thisTab = document.getElementById(header);
                        thisTab.style.background = 'silver';
                        thisTab.style.fontWeight = 'bold';

                        // Save for next go'round
                        currShow=document.getElementById(target);
                        lastHeader = document.getElementById(header);

                        return false;
                    }

                    else if (IE) {

                        // Hide the last one, and flip the tab color back.
                        currShow.style.visibility = 'hidden';
                        if (lastHeader) { lastHeader.style.background = '#e0e0e0'; lastHeader.style.fontWeight='normal';}

                        // Show this one, and make the tab silver
                        document.all[target].style.visibility = 'visible';
                        document.all[header].style.background = 'silver';
                        document.all[header].style.fontWeight = 'bold';

                        // Save for next go'round
                        currShow=document.all[target];
                        lastHeader=document.all[header];

                        return false;
                    }

                    else if (NS4) {

                        // Hide the last one, and flip the tab color back.
                        currShow.visibility = 'hide';
                        // if (lastHeader) { lastHeader.bgColor = '#e0e0e0'; }

                        // Show this one, and make the tab silver
                        document.layers[target].visibility = 'show';
                        // document.layers[header].bgColor  = 'silver';

                        // Save for next go'round
                        currShow=document.layers[target];
                        // lastHeader=document.layers[header];

                        return false;
                    }

                    // && (version >=4)
                    else {
                        window.location=('#A' +target);
                        return true;
                    }


                }

                document.writeln('<style type=\"text/css\">');
                document.writeln('.tab-button	{ width: 70; height: 20; font-weight: normal; background: #e0e0e0; border-top: 1px solid buttonhighlight; border-left: 1px solid buttonhighlight; border-bottom: 1px solid buttonhighlight;  border-right: 1px solid buttonshadow; cursor: hand; padding-top: 2px;}');
                document.writeln('.tab-button-on	{ width: 70; height: 20; font-weight: bold;   background: silver; border-top: 1px solid buttonhighlight; border-left: 1px solid buttonhighlight; border-bottom: 1px solid buttonhighlight;  border-right: 1px solid buttonshadow; cursor: hand; padding-top: 2px;}');
                document.writeln('.tab-body	{ background: #d5d5d5; border-top: 0px solid buttonshadow; border-left: 1px solid buttonhighlight; border-bottom: 1px solid buttonshadow; border-right: 1px solid buttonshadow; padding: 10;    position:absolute; left:10px; top:35px; z-index:99; width:' + max_width + '; visibility: hidden}; overflow:hidden');
                document.writeln('</style>');
                document.writeln('<div id=\"tabs\" style=\"position:absolute; left:10px; top:15px; width:' + max_width + '; z-index:5;\">');

            </script>

                <table border='0' cellpadding='0' cellspacing='1'>
                  <tr valign='top'>
                   $headers
                  </tr>
                </table>
                </div>

                $content


                <script language='JavaScript'>
                   if (DOM) {var currShow=document.getElementById('T1');}
                   else if (IE) {var currShow=document.all['T1'];}
                   else if (NS4) {var currShow=document.layers['T1'];}

                   changeCont('1', 'tab1_head');
                </script>
         ";
				}

				function draw_admin_tree ( $title = '' , $treevars = '' )
				{
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $CGI;
						$color   = "#c0c0c0";
						$applist = "<a href='$SCRIPT_NAME?action=menu'>Applications List</a>";
						if ( $CGI[ hidelist ] ) {
								$applist = '';
						}
						echo "

            <head>
               <meta http-equiv='refresh' content='30' />
            </head>

            <script language='javascript'>
                var image_dir = '$config[button_dir]';
                var target = 'main';
            </script>

            <script src='$config[script_dir]/xtree.js'></script>

            <script language='javascript'>

                var tree = new WebFXTree('$title');

                tree.setBehavior('classic');

                $treevars

            </script>

            <style>
                table        { font-family: Arial; font-size: 10pt }
                td           { font-family: Arial; font-size: 10pt }
                tr           { font-family: Arial; font-size: 10pt }
                A            { text-decoration: none; color:navy; }
                A:visited    { text-decoration: none; color:navy; }
            </style>
            <body style='margin:0px; padding:0px; border:1px solid $color;background:$color;overflow:auto'>
                <div style='font-family:arial;font-size:9pt;'>
                        <script language='javascript'>
                           if (document.getElementById) {
                               document.write(tree);
                           }
                        </script>
                </div>
            </body>
         ";
						exit;
				}

				function javascript ()
				{
						global $CGI;
						global $config;
						global $SCRIPT_NAME;
						$code        = $CGI[ code ];
						$base_js     = "$SCRIPT_NAME?action=javascript&code=";
						$none_image  = $config{button_dir} . "none.gif";
						$images_base = $config{button_dir} . "/tree/";
						$jssource    = array ();
						# ------- Return Proper Javascript to the Browser -------- #
						$js = $jssource[ $code ];
						$js = preg_replace ( "/(<<BASE_JS>>)/" , $base_js , $js );
						$js = preg_replace ( "/(<<NONE_IMAGE>>)/" , $none_image , $js );
						$js = preg_replace ( "/(<<IMAGES_BASE>>)/" , $images_base , $js );
						echo $js;
						exit;
				}
		} // end class


?>
