<?php

    /*
      ________________________________________________________________________________
      
        PROGRAM : ds/X Application Setup
        VERSION : 1.00
        CVS     : $Revision: 1.2 $
      
        Author  : John Cokos, iWeb, Inc.
      ________________________________________________________________________________
                                                                                       
                                                                                       
                 _______/\/\______________________/\/\__/\/\____/\/\_
                 _______/\/\____/\/\/\/\________/\/\______/\/\/\/\___
                 ___/\/\/\/\__/\/\/\/\________/\/\__________/\/\_____
                 _/\/\__/\/\________/\/\____/\/\__________/\/\/\/\___
                 ___/\/\/\/\__/\/\/\/\____/\/\__________/\/\____/\/\_
                 ____________________________________________________

                      
                                     ds/X: Perl/PHP/ASP/mySQL API (c) 2001 iWeb, Inc.     
                                                                                       
      ________________________________________________________________________________
         Copyright and Legal Notices:                                                  
                                                                                       
           All source code, images, programs, files included in this distribution      
           Copyright (c) 1996 - 2003
                                                                                       
                  John C. Cokos, The CCS Network, Inc.  iWeb, Inc.                     
                  All Rights Reserved.                                                 
                                                                                       
           Use, distibution, sale, or access to this program is forbidden without      
           permission, except were permitted by the license agreement.                 
                                                                                       
      ________________________________________________________________________________
         Special Instructions and notes...
      
      ________________________________________________________________________________

   */


   class installAdmin {

       function installAdmin() { 

           global $data_dir;
           global $CGI;
           global $config;
           global $include_dir;

           if ( file_exists( $include_dir . "/dsX/iSQL/drivers/mysql.inc") ) { include_once( $include_dir . "/dsX/iSQL/drivers/mysql.inc"); }

           include_once( "$data_dir/config.inc" );

           if ( $CGI['action'] != 'go_to_admin' && file_exists( "$data_dir/.complete" ) ) { 
               $stamp = implode( "", file("$data_dir/.complete") );
               $stamp = preg_replace("/\s+|\cM|\n|\r/", "", $stamp);
               $date = date("m/d/Y h:i", $stamp);
               throw_error(E_USER_ERROR, "Setup Completed on $date", "admin.php", "1");
               exit;
           }
       }


       // ---------------------------------------------------------------------------- //
       // Run desired Action
       // ---------------------------------------------------------------------------- //

       function go() {

           global $config;
           global $data_dir;
           global $CGI;

           // Read-Set Config File
           foreach ( $config as $option=>$val ) {  
              if ( ! empty($CGI[$option]) ) { $config[$option] = $CGI[$option]; }
           }

           // If we're not just getting started, save what we have so far //
           if ( $CGI['action'] ) { 
               // Save the file: This will catch any new settings added, and ensure permissions at the start

               list($ok, $message) = $this->save_config();

               if ( ! $ok ) { 
                   $output .= "
                     Global Settings File <b>could not</b> be modified.
                     <br /><br />
                     <code style='color:red'>$message</code>
                     <br /><br />
                     Please check that the 'data' directory (<code>$data_dir</code>) has '777' permissions
                     throughout (specifically the '<b>config.inc</b>' file).  Once you have done
                     this, press the 'RELOAD' button in your browser to try again.
                     <br /><br />
                     <b>DO NOT</b> hit the 'Next' button until this issue is resolved.
                   ";
                   
                   $this->setup( array("title"=>"Permissions Error", "output"=>$output, "action"=>$CGI['action']) );
                   exit;
               }
           }
           else { $CGI['action'] = "welcome"; }           

           $action = $CGI['action'];

           if ( method_exists($this,$action) ) { 
                call_user_func( array(&$this,$action) ); 
           } 
           else { trigger_error("ERROR", "Invalid Command: $action"); }
         
           exit;

       }


       // ---------------------------------------------------------------------------- //
       // Fancy Output
       // ---------------------------------------------------------------------------- //
       function setup( $args = array() ) {
           
           global $SCRIPT_NAME;
           
           $title = $args['title'];
           $output = $args['output'];
           $next_action = $args['action'];

           echo "
                <html>
                  <head>
                     <title>$title</title>
                     <style>
                       body, td, div, input { font-family:verdana,arial,helvetica;font-size:9pt; }
                       li { padding-bottom:10px; }
                     </style>
                  </head>

                <body>

                <table width='700' cellspacing='0' align='center' style='border:1px ridge silver'>
                  <tr>
                    <td style='background:#596E88; padding:8px; color:white'>
                       <b>$title</b>
                    </td>
                  </tr>
                  <tr>
                    <td style='padding:10px'>
                      <form action='$SCRIPT_NAME' method='post'>
                    
                         $output

                         <div style='text-align:right'>
                         <input type='hidden' name='action' value='$next_action' />
                         <input type='hidden' name='admin' value='install' />
                         <input type='submit' value='Next &gt;&gt;' />
                         </div>

                      </form> 
                    </td>
                  </tr>
                </table>

                </body>
                </html>
           ";
           
           exit;
       }


       // ---------------------------------------------------------------------------- //
       // Welcome: Basic Information
       // ---------------------------------------------------------------------------- //
       function welcome() { 

           global $config;

           $title = "Application Setup";
           $next = "get_database_info";

           $output = "
               Before we can run the actual setup, please read/do the following:
                   
               <ul>
                  <li><b>If you have moved your 'data' directory</b> ...
                      Edit the <b>settings.php</b> file located in the base installation directory.
                      <br /><br >
                      In this file, you will need to change 1 variable:
                      <br /><b>data_dir</b></b><br />
                      The location of these variables, and instructions for changing them
                      are clearly marked within the file.</li>   
                   <li><b>If you have NOT moved your 'data' directory</b> ... <br />
                      Simply press the 'Next' button to continue.
                      <br /><br >
               </ul>
               Once you have completed this step (if  necessary), you may proceed with the setup process
               by clicking the 'Next' button below.
           ";

           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );

       }

       // ---------------------------------------------------------------------------- //
       // Step 1: Basic SQL Information
       // ---------------------------------------------------------------------------- //
       function get_database_info( $error='' ) { 

           global $config;

           $title = "Step 1: Gather Database Information";
           $next = "test_database";

           $form = "
               $error
               <b>Please enter the information required to connect to your SQL Database
               in the spaces provided below.</b>
               <br /><br />
               <table cellspacing='5'>
                  <tr>
                     <td>Hostname</td>
                     <td><input name='dbhost' value='$config[dbhost]' /></td>
                  </tr>
                  <tr>
                     <td>Database Name</td>
                     <td><input name='dbname' value='$config[dbname]' /></td>
                  </tr>
                  <tr>
                     <td>User Name</td>
                     <td><input name='dbusername' value='$config[dbusername]' /></td>
                  </tr>
                  <tr>
                     <td>Password</td>
                     <td><input name='dbpassword' value='$config[dbpassword]' /></td>
                  </tr>
               </table>
           ";

           $this->setup( array("title"=>$title, "output"=>$form, "action"=>$next) );

       }

       // ---------------------------------------------------------------------------- //
       // Step 2: Connect, Test Database .... 
       // ---------------------------------------------------------------------------- //
       function test_database() { 

           global $config;
           global $data_dir;

           $title = "Step 2: Database Settings Verification";
           $next = "create_database";

           $iSQL = new iSQL ( $config["dbhost"], $config["dbname"], $config["dbusername"], $config["dbpassword"], $config['dbwritehost'], $config['dbwritehost'], 1 );
           if ( $iSQL->error ) { 
               $this->get_database_info("<div style='color:red; padding:8px;'>Error: <b>$iSQL->error</b>, please check your settings</div>");
           }

           $output = "";

           $output .= "Checking Create Table ... ";
           $SQL = "CREATE TABLE test ( testcolumn varchar(10) )";
           $iSQL->run($SQL,1);
           if ( $iSQL->error ) { $output .= "<span style='color:red'>Failed ($iSQL->error)</span><br />"; }
           else { 
               $output .= "<span style='color:blue'>OK</span><br />";

               $output .= "Checking Create Index ... ";
               $SQL = "CREATE INDEX t1 ON test(testcolumn)";
               $iSQL->run($SQL,1);
               if ( $iSQL->error ) { $output .= "<span style='color:red'>Failed ($iSQL->error)</span><br />"; }
               else { 
                   $output .= "<span style='color:blue'>OK</span><br />";

                   $output .= "Checking DROP Table ... ";
                   $SQL = "DROP TABLE test";
                   $iSQL->run($SQL,1);
                   if ( $iSQL->error ) { $output .= "<span style='color:red'>Failed ($iSQL->error)</span><br />"; }
                   else { 
                       $output .= "
                         <span style='color:blue'>OK</span><br />
                         <br />
                         Database Connection and permissions seem to be ok.  Settings Saved.
                         <br /><br />
                         Click the 'Next' Button to create the system tables and default data
                       ";
                   }
               }
           }


           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );
       }   

       // ---------------------------------------------------------------------------- //
       // Step 3: Create Tables & put in default data
       // ---------------------------------------------------------------------------- //
       function create_database() { 

           global $config;
           global $data_dir;

           $title = "Step 3: Create Database / Load Data";
           $next = "get_paths";

           $iSQL = new iSQL ( $config["dbhost"], $config["dbname"], $config["dbusername"], $config["dbpassword"], $config['dbwritehost'], $config['dbwritehost'], 1 );
           if ( $iSQL->error ) { 
               get_database_info("<div style='color:red; padding:8px;'>Error: <b>$iSQL->error</b>, please check your settings</div>");
           }

           $dir = $data_dir . "/sql/schemas/";
           $create = array();
           $data = array();

           if ($handle = opendir($dir)) {
               while (false !== ($file = readdir($handle))) { 
                  if ( preg_match("/\.done/i",$file) ) { continue; }
                  if ( preg_match("/\.mysql/i",$file) ) { array_push($create, $file); }
                  if ( preg_match("/\.data/i",$file) ) { array_push($data, $file); }
                  if ( preg_match("/\.patch/i",$file) ) { 
                       $patchname = $dir . "/" . $file;
                       $ignorename = $patchname . ".done";
                       rename($patchname,$ignorename);
                  }
               }
               closedir($handle); 
           }
           else { 
               $output = "<span style='color:red'>Could not open SQL Definitions (<code>$dir</code>)</span><br /><br />Please check that directory and try this step again";
               $next = "create_database";
               $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );
           }
           
           $output = "";

           foreach ( $create as $cfile ) {
               $filename = $dir . $cfile; 
               $pname = preg_replace("/\.mysql/", "", $cfile);
               $sql = implode("", file($filename));
               $output .= "Create Tables ($pname)";
               $statements = split('#STMT#',$sql);
               foreach ( $statements as $SQL ) { 
                   $SQL = preg_replace("/^\s+|\s+$/", "", $SQL);
                   if ( ! $SQL ) { continue; }
                   $iSQL->run($SQL, 1);
                   if ( $iSQL->error ) { 
                       $output .= "<br /><span style='color:red'>Failed ($iSQL->error)</span><br /><br /><code>[$SQL]</code><br /><br />Please try again when the problem is fixed."; 
                       $next = "create_database";
                       $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );
                   }
                   else { $output .= "."; }
               }
               $output .= "<br />";

               $newfilename = $filename . ".done";
               rename($filename, $newfilename);
           }

           $output .= "<hr />";

           foreach ( $data as $cfile ) {
               $filename = $dir . $cfile; 
               $pname = preg_replace("/\.mysql/", "", $cfile);
               $sql = implode("", file($filename));
               $output .= "Inserting Default Data ($pname)<br />";
               $statements = split('#STMT#',$sql);
               $cnt = 0;
               foreach ( $statements as $SQL ) { 
                   $SQL = preg_replace("/^\s+|\s+$/", "", $SQL);
                   if ( ! $SQL ) { continue; }
                   $iSQL->run($SQL, 1);
                   if ( $iSQL->error ) { 
                       $output .= "<br /><span style='color:red'>Failed ($iSQL->error)</span><br /><br /><code>[$SQL]</code><br /><br />Please try again when the problem is fixed."; 
                       $next = "create_database";
                       $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );
                   }
                   else { $output .= "."; }
                   $cnt++;
                   if ( $cnt == 100 ) { $output .= "<br />\n"; $cnt = 0; }
               }
               $output .= "<br />";

               $newfilename = $filename . ".done";
               rename($filename, $newfilename);
           }


           $output .= "
             <hr />
             <b>
             Your database has been created, and initial settings were saved.
             <br /><br />
             Click the 'Next' Button to continue with the setup process.
             </b>
           ";

           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );
       }   

       // ---------------------------------------------------------------------------- //
       // Step 4: Gather Path Information
       // ---------------------------------------------------------------------------- //
       function get_paths() { 

           global $config;
           global $SCRIPT_NAME;
           global $HTTP_HOST;

           $title = "Path Info";
           $next = "additional_options";

           $pathname = preg_replace("/admin\.php/", "", $SCRIPT_NAME);
           $url = "http://" . $HTTP_HOST . $pathname;

           if ( ! $config['button_dir'] ) { $config['button_dir'] = $pathname . "images"; }
           if ( ! $config['script_dir'] ) { $config['script_dir'] = $pathname . "include"; }
           if ( ! $config['hyperseek_name'] ) { $config['hyperseek_name'] = $pathname . "search.php"; }
           if ( ! $config['redirect_url'] ) { $config['redirect_url'] = $url . "r.php"; }
           if ( ! $config['search_url'] ) { $config['search_url'] = $url . "search.php"; }
           if ( ! $config['jackhammer_accounts_url'] ) { $config['jackhammer_accounts_url'] = $url . "accounts.php"; }
           if ( ! $config['affjump_url'] ) { $config['affjump_url'] = $url . "cobrand.php"; }
           if ( ! $config['ADMIN_DIR'] ) { $config['ADMIN_DIR'] = $pathname; }           
           
           $output = "
               <b>Now, we need to set up some of the path names for the various files</b>
               <br /><br />
               Below are the default (or previously entered) settings.  Please change
               according to your actual installation's virtual path names.
               <br /><br />

               <table cellspacing='5'>
                  <tr>
                     <td>Virtual URL for your 'images' folder:</td>
                     <td><input name='button_dir' value='$config[button_dir]' /></td>
                  </tr>
                  <tr>
                     <td colspan='2'>
                        <i>This is the <b>images</b> folder that was included as a part of your installation, not your website's regular images folder</i>
                     </td>
                  </tr>
                  
                  <tr>
                     <td>Virtual URL for your 'include' folder:</td>
                     <td><input name='script_dir' value='$config[script_dir]' /></td>
                  </tr>
                  <tr>
                     <td colspan='2'>
                        <i>This is the <b>include</b> folder that was included as a part of your installation, most likely inside the same directory as your 'images' folder from above.</i>
                     </td>
                  </tr>

                  <tr>
                     <td>FULL URL (with http://) for main search program (search.php):</td>
                     <td><input name='hyperseek_name' value='$config[search_url]' /></td>
                  </tr>

                  <tr>
                     <td>FULL URL (with http://) for your 'r.php' file:</td>
                     <td><input name='redirect_url' value='$config[redirect_url]' /></td>
                  </tr>

                  <tr>
                     <td>FULL URL (with http://) for your 'accounts.php' file:</td>
                     <td><input name='jackhammer_accounts_url' value='$config[jackhammer_accounts_url]' /></td>
                  </tr>

                  <tr>
                     <td>FULL URL (with http://) for your 'cobrand.php' file:</td>
                     <td><input name='affjump_url' value='$config[affjump_url]' /></td>
                  </tr>

                  <tr>
                     <td>Virtual URL for your 'admin' directory:</td>
                     <td><input name='ADMIN_DIR' value='$config[ADMIN_DIR]' /></td>
                  </tr>

               </table>
           ";
           
           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );

       }


       // ---------------------------------------------------------------------------- //
       // Step 5: Get some additional options
       // ---------------------------------------------------------------------------- //
       function additional_options() { 

           global $config;

           $title = "Additional Settings";
           $next = "admin_password";
           
           $output = "
               <b>Just a few more generic settings ....</b>
               <br /><br />
               Below are the default (or previously entered) settings.  Please change
               according to your preferences
               <br /><br />

               <table cellspacing='5'>
                  <tr>
                     <td>Your (admin) email address:</td>
                     <td><input name='email' value='$config[email]' /></td>
                  </tr>
                  <tr>
                     <td colspan='2'>
                        <i>Used as the 'from' address on all system outbound emails</i>
                     </td>
                  </tr>
                  
                  <tr>
                     <td>Allowed Domains for link submission</td>
                     <td><input name='yh_domains' value='$config[yh_domains]' /></td>
                  </tr>
                  <tr>
                     <td colspan='2'>
                        <i>Enter every IP Number and domain name (both with and without the 'www') that you will allow people to submit new listings <b>FROM</b>.  This can ensure that people can only add a link from a form on your site, or if you enter it here, a remotely hosted form (such as a link submission service) that will submit to you from afar.  Separate entries with a space.</i>
                     </td>
                  </tr>

               </table>
           ";
           
           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );

       }


       // ---------------------------------------------------------------------------- //
       // Step 6: Set the Admin Password
       // ---------------------------------------------------------------------------- //
       function admin_password() { 

           global $config;

           $title = "Set Admin Password";
           $next = "finished";
           
           $output = "
               <b>Select a password for use in gaining access to the admininistrative control center.</b>
               <br /><br />
               Choose a password that is difficult to guess, but one that you can remember.  It's recommended
               that you <b>do not</b> use the same password here that you use for mySQL or your server's FTP
               access.  Having different passwords in all 3 areas minimizes the risk of a large scale breech,
               (should someone be able to guess a common password like that, they'd have access to all aspects of your server)
               <br /><br />

               <table cellspacing='5'>
                  <tr>
                     <td>Administrative Password:</td>
                     <td><input name='admin_pw' value='$config[admin_pw]' /></td>
                  </tr>
                </table>

           ";
           
           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );

       }


       // ---------------------------------------------------------------------------- //
       // Step 7: Finish Up
       // ---------------------------------------------------------------------------- //
       function finished() { 

           global $HTTP_HOST;
           global $SCRIPT_NAME;
           global $config;
           global $data_dir;

           $admin_url = "http://" . $HTTP_HOST . $SCRIPT_NAME;

           $title = "All Finished";
           $next = "go_to_admin";
           
           $output = "
              <b>Setup is Complete</b>
              <br /><br />
              The Database is Connected, and the most basic and essential settings have
              been set.   You will need to complete a few more steps once you are
              in the admin center:
              <ul>
                 <li>Go through all of the 'Preferences' pages, to set the options
                     that were not asked for during this basic setup program.
                 <li>Go through all of the 'Language -> Word Translations' as well,
                     to ensure that the various parts of the output are worded according
                     to your liking.   Note that as you're going through the system, most
                     wording that you see comes from this section, so it should be the
                     first place you look when you want to change wording.  You will also
                     find the Internal Templates, and the Table Schemas areas helpful
                     in tweaking layouts and wordings for specific parts of the system.
              </ul>
              <hr />
              Click the next button to load your admin center.  You will be presented
              with a pop-up password dialogue, requiring you to login so that you may
              enter the Administrative Application.
              <br /><br />
              <font size='+2'>
              Login ID: <b>admin</b><br />
              Password: <b>$config[admin_pw]</b><br /><br />
              </font>

              For reference, your Admin Center URL is:
              <br /><br />
                  <code><b>$admin_url</b></code>
              <hr />
           ";

           write_file("$data_dir/.complete", time());

           $this->setup( array("title"=>$title, "output"=>$output, "action"=>$next) );


       }

       // ---------------------------------------------------------------------------- //
       // Step 7: Finish Up
       // ---------------------------------------------------------------------------- //
       function go_to_admin() { 
           global $SCRIPT_NAME;
           $SCRIPT_NAME = preg_replace("/setup\.php/", "index.php", $SCRIPT_NAME);
           header("Location: $SCRIPT_NAME");
           exit;
       }

       // ---------------------------------------------------------------------------- //
       // Save config.inc file
       // ---------------------------------------------------------------------------- //
       function save_config() {
           
           global $config;
           global $data_dir;

           $file = "$data_dir/config.inc";

           $cfg_dump = var_export($config,1);
     
           $output = "<?php\n\n";
           $output .= 'global $config;' . "\n\n";
           $output .= '$config = ' . $cfg_dump;
           $output .= "\n\n";
           $output .= "?>";

           return write_file( $file, $output);

       }

   } // End Class

?>
