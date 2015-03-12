<?php

		/*
				________________________________________________________________________________

						  PROGRAM : ASP : Admin : SQL Schema Admin Center (PHP)
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


		class accountsAdmin
		{

				var $output;

				function accountsAdmin ()
				{
				}


				################################################################################
				# Run the requested action
				################################################################################
				function go ()
				{
						global $CGI;
						$action = $CGI[ 'action' ];
						if ( !$action ) {
								$action = "mainmenu";
						}
						if ( method_exists ( $this , $action ) ) {
								call_user_func ( array ( &$this , $action ) );
						} elseif ( function_exists ( $action ) ) {
								call_user_func ( $action );
						} else {
								admin_output ( "ERROR" , "Invalid Action: $action" );
								exit;
						}
				}

				function mainmenu ()
				{
						admin_output ( "Program Configuration" , "Testing SQL Admin" );
				}

				function help ()
				{
				}


				// --------------------------------------------------------------------- //
				//  Do User Lookups
				// --------------------------------------------------------------------- //
				function search ( $return = '' )
				{
						ini_set ( 'memory_limit' , '2048M' );
						set_time_limit ( 120 );
						global $SCRIPT_NAME;
						global $language;
						global $oSQL;
						$SQL           = "SELECT DISTINCT group_name FROM " . USER_GROUPS_TABLE;
						$groups        = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						$group_options = '';
						foreach ( $groups as $group ) {
								$group_options .= "<option value='$group'>$group</option>";
						}
						$form = "

              <table style='margin:0px;border:1px solid black;' border='0' cellspacing='0' cellpadding='3'>

                <form action = '$SCRIPT_NAME' target='main'>

                <tr>
                   <td style='border-bottom: 1px solid silver' valign='top' colspan=2>
                         <span style='font-size:11pt;font-weight:bold'>$language[main_title_bar]</span>
                    </td>
                </tr>

                <tr>
                    <td valign='top'>$language[acct_balance_prompt]</td>
                    <td valign='top'>
                         <select name='balance_qualifier'>
                             <option value='greater'>$language[acct_balance_higher]</option>
                             <option value='lesser'>$language[acct_balance_lower]</option>
                         </select>
                         <input  name='balance' value='' size=8>
                     </td>
                </tr>

                <tr>
                    <td valign='top'>Account Belongs to Group:</td>
                    <td valign='top'>
                         <select name='group'>
                             <option value=''>Any Group</option>
                             $group_options
                         </select>
                     </td>
                </tr>

                <tr>
                    <td valign='top'>$language[acct_id_prompt]</td>
                    <td valign='top'><input name='acct_id' size=10></td>
                </tr>

                <tr>
                    <td valign='top'>eMail</td>
                    <td valign='top'><input name='contact_email' size=10></td>
                </tr>

                <tr>
                    <td colspan='2' align='center'>
                         <input type='hidden' name='action' value='list_accounts'>
                         <input type='hidden' name='admin' value='accounts'>
                         <input type='submit' value='$language[search_users_link]'>
                    </td>
                </tr>

              </form>
              </table>


            ";
						if ( $return ) {
								return $form;
						}
						admin_output ( $language[ acct_list_title ] , $form );
				}


				// --------------------------------------------------------------------- //
				//  List Accounts found by search
				// --------------------------------------------------------------------- //
				function list_accounts ()
				{
						ini_set ( 'memory_limit' , '2048M' );
						set_time_limit ( 120 );
						global $oSQL;
						global $CGI;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						$AND           = "";
						$id            = '';
						$email         = '';
						$accounts_list = '';
						if ( $CGI[ acct_type ] == "Inactive" ) {
								$CGI[ acct_type ] = '';
								$bad_date         = time () - 5184000; // 60 days
								$AND              = " AND  last_visit < $bad_date ";
						}
						if ( $CGI[ acct_id ] ) {
								$id = " AND acct_id LIKE '$CGI[acct_id]%' ";
						}
						if ( $CGI[ contact_email ] ) {
								$email .= " AND contact_email LIKE '%$CGI[contact_email]%'";
						}
						if ( $CGI[ 'group' ] ) {
								$SQL            = "SELECT DISTINCT acct_id,1 FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name = '$CGI[group]'";
								$accts          = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
								$valid_accounts = "'" . join ( "','" , $accts ) . "'";
								$GROUP_AND      = " AND acct_id IN($valid_accounts)";
						}
						$WHERE = "WHERE 1=1 $id $email";
						if ( $CGI[ balance ] ) {
								$qual = ">=";
								if ( $CGI[ balance_qualifier ] == "lesser" ) {
										$qual = "<=";
								}
								$CGI[ balance ] = preg_replace ( "/\$/" , "" , $CGI[ balance ] );
								$AND .= " AND current_balance $qual $CGI[balance] ";
						}
						$SQL     = "SELECT * FROM " . ACCOUNTS_TABLE . " $WHERE $AND $GROUP_AND ORDER BY acct_id ASC";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$shown   = 0;
						$total   = count ( $results );
						$page    = $CGI[ page ];
						if ( !$page ) {
								$page = 1;
						}
						$start = $page - 1;
						$start = $start * 50;
						if ( !$start ) {
								$start = 1;
						}
						$end = $start + 49;
						if ( $end > $total ) {
								$end = $total;
						}
						foreach ( $results as $account ) {
								$shown++;
								if ( $shown < $start ) {
										continue;
								}
								if ( $shown > $end ) {
										break;
								}
								$manage_bids  = "
                   <a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&acct_id=$account[acct_id]'>$language[manage_bids_link]</a>
                ";
								$SQL          = "SELECT count(*) FROM " . AFF_INFO_TABLE . " WHERE affiliate = '$account[acct_id]'";
								$is_affiliate = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								$afflink      = $language[ 'add_affiliate_link' ];
								if ( $is_affiliate ) {
										$afflink = $language[ 'edit_affiliate_link' ];
								}
								$affiliate_link = "
                   <a href='$SCRIPT_NAME?admin=accounts&action=affiliate_edit&affiliate=$account[acct_id]'>$afflink</a>
                ";
								$accounts_list .= "
                      <tr>
                            <td>
                               <a href='$SCRIPT_NAME?admin=accounts&action=edit_account&acct_id=$account[acct_id]'>$language[acct_edit]</a>
                               <a href='$SCRIPT_NAME?admin=accounts&action=acct_groups&acct_id=$account[acct_id]'>$language[acct_group_edit]</a>
                               $manage_bids
                               $affiliate_link
                            </td>
                            <td>
                               <a href='$config[jackhammer_accounts_url]?login_userid=$account[acct_id]&login_password=$account[acct_password]' target='new'>$account[acct_id]</a>
                            </td>
                            <td>
                               $account[acct_password]
                            </td>
                            <td>
                               $account[contact_name]
                            </td>
                            <td>
                               <a href='mailto:$account[contact_email]'>$account[contact_email]</A>
                            </td>
                            <td>
                               $language[dollar_sign]$account[current_balance]
                               <A HREF='$SCRIPT_NAME?admin=accounts&action=adjust_balance&acct_id=$account[acct_id]'>+/-</A>
                               <A HREF='$SCRIPT_NAME?admin=accounts&action=deposit_history&acct_id=$account[acct_id]'>Hist</a>
                            </td>
                            <td>
                               $language[dollar_sign]$account[total_paid]
                            </td>
                      </tr>
                  ";
						}
						$form = $this->search ( 1 );
						## Navigation ##
						if ( $total > 50 ) {
								$navlinks = array ();
								$numpages = ceil ( $total / 50 );
								for ( $x = 1; $x <= $numpages; $x++ ) {
										if ( $x == $page ) {
												array_push ( $navlinks , "<b>$x</b>" );
										} else {
												array_push ( $navlinks , "<a href='$SCRIPT_NAME?admin=accounts&balance_qualifier=$CGI[balance_qualifier]&balance=$CGI[balance]&acct_id=$CGI[acct_id]&contact_email=$CGI[contact_email]&action=list_accounts&page=$x'>$x</a>" );
										}
								}
								$navbar = "... Page: [ " . join ( " | " , $navlinks ) . " ]";
						} else {
								$navbar = '';
						}
						$output = "

               $form

               <div style='padding:15px'>
               $start to $end of $total
               $navbar
               </div>

               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

                <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td style='width: 60px;'>$language[header_actions]</td>
                        <td style='width: 60px;'>$language[header_user_id]</td>
                        <td style='width: 60px;'>$language[header_password]</td>
                        <td style='width: 60px;'>$language[header_name]</td>
                        <td style='width: 60px;'>$language[header_email]</td>
                        <td style='width: 60px;'>$language[header_balance]</td>
                        <td style='width: 60px;'>$language[header_paid]</td>
                    </tr>
                </thead>
                <tbody>

                <tbody>
                   $accounts_list
                </tbody>
                </table>

               <div style='padding:15px'>
               $start to $end of $total
               $navbar
               </div>
            ";
						admin_output ( $language[ acct_list_title ] , $output );
				}


				// --------------------------------------------------------------------- //
				//  Edit account record
				// --------------------------------------------------------------------- //
				function edit_account ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						if ( $CGI[ action ] == "add_account" ) {
								$record = new Record( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE ) );
						} else {
								$record = new Record( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						}
						if ( !$record->data[ create_date ] ) {
								$record->data[ create_date ] = date ( "Ymd" );
						}
						$formarray  = $record->generate_form ();
						$formfields = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$formfields .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$name   = $record->data[ contact_name ];
						$output = $this->edit_menu () . "
                <br />
                <blockquote>
                  <form action='$SCRIPT_NAME' method='post'>
                  <table cellpadding=8 cellspacing=0 border=0>
                    <tr bgcolor='navy'><td colspan=2>
                      <font color='yellow'>
                         $language[edit_acct_title] <b>$name</b>
                      </font>
                    </td></tr>
                    $formfields
                    <tr><td colspan=2>
                       <center>
                         <select name='action'>
                            <option value='save_account'>$language[save_button]</option>
                            <option value='delete_account'>$language[delete_button]</option>
                         </select>
                         <input type='submit' value='Go'>
                       </center>
                    </td></tr>
                   </table>
                   <input type='hidden' name='admin' value='accounts' />
                 </form>
                 </blockquote>
           ";
						admin_output ( "$language[edit_acct_title] <b>$name</b>" , $output );
				}

				function edit_account_whitelisting ()
				{
						global $ccodes;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						global $oSQL;
						global $LICENSE_TYPE;
						global $config;
						if ( $CGI[ 'del' ] == "y" ) //Delete entry
						{
								$SQL = "delete from iweb_affiliate_blacklist where id={$CGI['id']}";
								$oSQL->run ( $SQL );
						}
						// Tables
						$accounts = ACCOUNTS_TABLE;
						$affinfo  = AFF_INFO_TABLE;
						$SQL      = "
                SELECT $accounts.*, $affinfo.*
                FROM $accounts, $affinfo
                WHERE $affinfo.affiliate = $accounts.acct_id
                ORDER BY $accounts.acct_id ASC
            ";
						$results  = $oSQL->search ( array ( "SQL" => $SQL ) );
						$OUTPUT   = $this->edit_menu ();
						$OUTPUT .= "
                <form action='$SCRIPT_NAME?action=edit_account_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}' method='post' id='edit-wl'>
                <table bgcolor='black' align='center' cellspacing='1' cellpadding='5' width='80%'>
                   <tr bgcolor='silver'>
                     <td colspan='2'>
                         <b>Whitelist Management for Account: $CGI[acct_id]</b>
                     </td>
                   </tr>
                   <tr bgcolor='silver'>
                     <td colspan=2>Publishers</td>
                   </tr>
                   <tr bgcolor='white'>
                      <td>Navigate:</td>
                      <td><select name='publisher' id='publisher' onchange=\"window.location.href='$SCRIPT_NAME?action=edit_account_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}&publisher='+document.getElementById('publisher').value\">
                        <option value=''></option>";
						foreach ( $results as $account ) {
								$OUTPUT .= "<option value='{$account['affiliate']}'";
								if ( $CGI[ 'publisher' ] == $account[ 'affiliate' ] ) $OUTPUT .= " SELECTED ";
								$OUTPUT .= ">{$account['affiliate']}</option>";
						}
						$OUTPUT .= "
					</select></td></tr>";
						if ( $CGI[ 'publisher' ] != "" ) {
								$SQL     = "
	                SELECT subid
                	FROM iweb_jh_affiliate_summary
            	    WHERE acct_id='{$CGI[publisher]}'
        	        GROUP BY subid
    	        ";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								$OUTPUT .= "
                   <tr bgcolor='white'>
                      <td>Unlisted publishers:</td>
                      <td>";
								$OUTPUT .= "<select name='subid'>
                      		<option value=''>Select SUBID</option>
                      		<option value='ALL'>Entire Account</option>";
								foreach ( $results as $account ) {
										$OUTPUT .= "<option value='{$account['subid']}'>{$account['subid']}</option>";
								}
								$OUTPUT .= "</select>";
								$OUTPUT .= "</td></tr>";
						}
						$OUTPUT .= "
                   <tr bgcolor='white'>
                      <td colspan='2' align='center'>
                         <input type='submit' name='type' value='Blacklist'>
                         <input type='submit' name='type' value='Whitelist'>
                         <input type='hidden' name='action' value='save_wl_account'>
                         <input type='hidden' name='admin' value='accounts'>
                         <input type='hidden' name='acct_id' value='$CGI[acct_id]'>
                      </td>
                   </tr>
                   <tr bgcolor='silver'><td colspan=2>Current Whitelists/Blacklists</td></tr>
                   <tr bgcolor='white'><td colspan=2>";
						$SQL     = "
	                SELECT *
                	FROM iweb_affiliate_blacklist
            	    WHERE engine='{$CGI[acct_id]}' and ad_id=0";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$OUTPUT .= "
	            	<table>
                   <tr>
                      <th style='border:solid 1px'>Publisher</th>
                      <th style='border:solid 1px'>Subid</th>
                      <th style='border:solid 1px'>Type</th>
                      <th style='border:solid 1px'>Action</th>
                    </tr>";
						foreach ( $results as $account ) {
								if ( $account[ 'type' ] == "bl" ) $type = "Blacklist"; else
										$type = "Whitelist";
								$OUTPUT .= "<tr bgcolor='white' style='border:solid 1px'>";
								$OUTPUT .= "<td>{$account['acct_id']}</td><td>{$account['subid']}</td>";
								$OUTPUT .= "<td>{$type}</td>";
								$OUTPUT .= "<td><a href='$SCRIPT_NAME?action=edit_account_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}&del=y&id={$account['id']}'>Remove</a></td>";
								$OUTPUT .= "</tr>";
						}
						$OUTPUT .= "</table>";
						$OUTPUT .= "</td></tr></table></form>";
						admin_output ( "Generate All" , $OUTPUT );
				}

				function edit_ad_whitelisting ()
				{
						global $ccodes;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						global $oSQL;
						global $LICENSE_TYPE;
						global $config;
						if ( $CGI[ 'del' ] == "y" ) //Delete entry
						{
								$SQL = "delete from iweb_affiliate_blacklist where id={$CGI['id']}";
								$oSQL->run ( $SQL );
						}
						// Tables
						$accounts = ACCOUNTS_TABLE;
						$affinfo  = AFF_INFO_TABLE;
						$OUTPUT   = $this->edit_menu ();
						$OUTPUT .= "
                <form action='$SCRIPT_NAME?action=edit_ad_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}' method='post' id='edit-wl'>
                <table bgcolor='black' align='center' cellspacing='1' cellpadding='5' width='80%'>
                   <tr bgcolor='silver'>
                     <td colspan='2'>
                         <b>Whitelist Management for Account: $CGI[acct_id]</b>
                     </td>
                   </tr>";
						$SQL     = "
	                SELECT iweb_hyperseek_textad.id as id, headline, camp_id
                	FROM iweb_hyperseek_textad, iweb_hyperseek_links
            	    WHERE account='{$CGI[acct_id]}' and iweb_hyperseek_textad.camp_id=iweb_hyperseek_links.id";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$OUTPUT .= "<tr bgcolor='white'>
                      			<td>Select Ad:</td><td>";
						$OUTPUT .= "<select name='ad_id' id='ad_id'>
                      		<option value=''>Select Ad</option>";
						foreach ( $results as $account ) {
								if ( $CGI[ 'ad_id' ] == $account[ 'id' ] ) $selected = "selected"; else
										$selected = "";
								$OUTPUT .= "<option value='{$account['id']}' {$selected}>{$account['camp_id']} - {$account['headline']}</option>";
						}
						$OUTPUT .= "</select></td></tr>";
						$SQL     = "
                SELECT $accounts.*, $affinfo.*
                FROM $accounts, $affinfo
                WHERE $affinfo.affiliate = $accounts.acct_id
                ORDER BY $accounts.acct_id ASC
            ";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$OUTPUT .= "
            		<tr bgcolor='silver'>
                     <td colspan=2>Publishers</td>
                   </tr>
                   <tr bgcolor='white'>
                      <td>Navigate:</td>
                      <td><select name='publisher' id='publisher' onchange=\"window.location.href='$SCRIPT_NAME?action=edit_ad_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}&publisher='+document.getElementById('publisher').value+'&ad_id='+document.getElementById('ad_id').value\">
                        <option value=''></option>";
						foreach ( $results as $account ) {
								$OUTPUT .= "<option value='{$account['affiliate']}'";
								if ( $CGI[ 'publisher' ] == $account[ 'affiliate' ] ) $OUTPUT .= " SELECTED ";
								$OUTPUT .= ">{$account['affiliate']}</option>";
						}
						$OUTPUT .= "
					</select></td></tr>";
						if ( $CGI[ 'publisher' ] != "" ) {
								$SQL     = "
	                SELECT subid
                	FROM iweb_jh_affiliate_summary
            	    WHERE acct_id='{$CGI[publisher]}'
        	        GROUP BY subid
    	        ";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								$OUTPUT .= "
                   <tr bgcolor='white'>
                      <td>Unlisted publishers:</td>
                      <td>";
								$OUTPUT .= "<select name='subid'>
                      		<option value=''>Select SUBID</option>
                      		<option value='ALL'>Entire Account</option>";
								foreach ( $results as $account ) {
										$OUTPUT .= "<option value='{$account['subid']}'>{$account['subid']}</option>";
								}
								$OUTPUT .= "</select>";
								$OUTPUT .= "</td></tr>";
						}
						$OUTPUT .= "
                   <tr bgcolor='white'>
                      <td colspan='2' align='center'>
                         <input type='submit' name='type' value='Blacklist'>
                         <input type='submit' name='type' value='Whitelist'>
                         <input type='hidden' name='action' value='save_wl_ad_account'>
                         <input type='hidden' name='admin' value='accounts'>
                         <input type='hidden' name='acct_id' value='$CGI[acct_id]'>
                      </td>
                   </tr>
                   <tr bgcolor='silver'><td colspan=2>Current Whitelists/Blacklists</td></tr>
                   <tr bgcolor='white'><td colspan=2>";
						$SQL     = "
	                SELECT *, iweb_affiliate_blacklist.id as id
                	FROM iweb_affiliate_blacklist, iweb_hyperseek_textad
            	    WHERE engine='{$CGI[acct_id]}' and iweb_affiliate_blacklist.ad_id=iweb_hyperseek_textad.id and ad_id<>0";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$OUTPUT .= "
	            	<table>
                   <tr>
                      <th style='border:solid 1px'>Publisher</th>
                      <th style='border:solid 1px'>Subid</th>
                      <th style='border:solid 1px'>Type</th>
                      <th style='border:solid 1px'>Ad</th>
                      <th style='border:solid 1px'>Action</th>
                    </tr>";
						foreach ( $results as $account ) {
								if ( $account[ 'type' ] == "bl" ) $type = "Blacklist"; else
										$type = "Whitelist";
								$OUTPUT .= "<tr bgcolor='white' style='border:solid 1px'>";
								$OUTPUT .= "<td>{$account['acct_id']}</td><td>{$account['subid']}</td>";
								$OUTPUT .= "<td>{$type}</td>";
								$OUTPUT .= "<td>{$account['camp_id']} - {$account['headline']}</td>";
								$OUTPUT .= "<td><a href='$SCRIPT_NAME?action=edit_ad_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}&del=y&id={$account['id']}'>Remove</a></td>";
								$OUTPUT .= "</tr>";
						}
						$OUTPUT .= "</table>";
						$OUTPUT .= "</td></tr></table></form>";
						admin_output ( "Generate All" , $OUTPUT );
				}

				function edit_menu ()
				{
						global $CGI;
						$ret = "<a href='$SCRIPT_NAME?action=edit_account&admin=accounts&acct_id={$CGI['acct_id']}'>Account Base</a> | ";
						$ret .= "<a href='$SCRIPT_NAME?action=edit_account_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}'>Account Whitelisting</a> | ";
						$ret .= "<a href='$SCRIPT_NAME?action=edit_ad_whitelisting&admin=accounts&acct_id={$CGI['acct_id']}'>Ad Whitelisting</a>";

						return $ret;
				}

				// --------------------------------------------------------------------- //
				//  Save Account Record
				// --------------------------------------------------------------------- //
				function save_account ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						$record = new Record( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										$message = $language[ record_saved ];
								} else {
										$message = "$language[record_save_error]<pre>$record->error_text</pre>";
								}
						} else {
								$message = "$language[record_save_error]<pre>$record->error_text</pre>";
						}
				}

				function save_wl_account ()
				{
						global $iSQL;
						global $CGI;
						$message = '';
						$acct_id = $CGI[ 'acct_id' ];
						if ( $CGI[ 'subid' ] == "" ) $blocker = $CGI[ 'camp' ]; else
								$blocker = $CGI[ 'subid' ];
						if ( $CGI[ 'type' ] == "Whitelist" ) $type = "wl"; elseif ( $CGI[ 'type' ] == "Blacklist" ) $type = "bl";
						else
								$type = "gl";
						$SQL = "insert into iweb_affiliate_blacklist set engine='{$CGI[acct_id]}', acct_id='{$CGI[publisher]}', subid='{$blocker}', type='{$type}'";
						$iSQL->run ( $SQL );
						$message = "Whitelist/Blacklist Information Saved";
						admin_output ( "$language[status_box_header]" , $message );
				}

				function save_wl_ad_account ()
				{
						global $iSQL;
						global $CGI;
						$message = '';
						$acct_id = $CGI[ 'acct_id' ];
						if ( $CGI[ 'subid' ] == "" ) $blocker = $CGI[ 'camp' ]; else
								$blocker = $CGI[ 'subid' ];
						if ( $CGI[ 'type' ] == "Whitelist" ) $type = "wl"; elseif ( $CGI[ 'type' ] == "Blacklist" ) $type = "bl";
						else
								$type = "gl";
						$SQL = "insert into iweb_affiliate_blacklist set engine='{$CGI[acct_id]}', acct_id='{$CGI[publisher]}', ad_id='{$CGI[ad_id]}', subid='{$blocker}', type='{$type}'";
						$iSQL->run ( $SQL );
						$message = "Whitelist/Blacklist Information Saved";
						admin_output ( "$language[status_box_header]" , $message );
				}


				// --------------------------------------------------------------------- //
				//  Delete Account Record
				// --------------------------------------------------------------------- //
				function delete_account ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						$record = new Record( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						if ( $record->delete () ) {
								$message = "Account $CGI[acct_id] deleted";
						} else {
								$message = "Delete Error: $record->error_text";
						}
						admin_output ( "$language[status_box_header]" , $message );
				}



				// --------------------------------------------------------------------- //
				//  List available user groups for select lists
				// --------------------------------------------------------------------- //
				function user_groups_list ()
				{
						global $oSQL;
						$grplist = "";
						$SQL     = "SELECT group_name FROM " . USER_GROUPS_TABLE . " order by group_name";
						$list    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
						foreach ( $list as $group ) {
								$grplist .= " <option value='$group'>$group</option> ";
						}

						return $grplist;
				}


				// --------------------------------------------------------------------- //
				//  List groups the selected account is a member of
				// --------------------------------------------------------------------- //
				function acct_groups ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						$SQL      = "SELECT group_name,1 FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE acct_id = '$CGI[acct_id]'";
						$assigned = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'fullhash' ) );
						$SQL      = "SELECT group_name FROM " . USER_GROUPS_TABLE;
						$groups   = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
						$list     = '';
						foreach ( $groups as $group ) {
								$checked = '';
								if ( $assigned[ $group ] ) {
										$checked = 'checked';
								}
								$list .= " <tr><td><input type='checkbox' name='assigned_groups[]' value='$group' $checked></td><td><b>$group</b></td></tr> ";
						}
						$output = "

               <b>$CGI[acct_id]</b>

                <form action='$SCRIPT_NAME' method='post'>
                <table>
                  <tr>
                    <td colspan=2>$language[acct_group_form_header]</td>
                  </tr>

                  $list

                  <tr>
                    <td colspan=2 align='center'>
                      <input type='hidden' name='acct_id' value='$CGI[acct_id]'>
                      <input type='hidden' name='action' value='save_acct_groups'>
                      <input type='hidden' name='admin' value='accounts'>
                      <input type='submit' value='$language[save_group_button]'>
                    </td>
                  </tr>
                </table>
                </form>

           ";
						admin_output ( "$language[edit_group_title] <b>$CGI[acct_id]</b>" , $output );
				}


				// --------------------------------------------------------------------- //
				//  Save Group membership for a single account
				// --------------------------------------------------------------------- //
				function save_acct_groups ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						## First, nuke any existing assignments ##
						$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE acct_id = '$CGI[acct_id]'";
						$iSQL->run ( $SQL );
						$split  = chr ( 129 );
						$report = '';
						$groups = $CGI[ assigned_groups ];
						foreach ( $groups as $group ) {
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => GROUP_ASSIGNMENTS_TABLE , "Schema" => GROUP_ASSIGNMENTS_TABLE , "Lookup" => 1 , "group_name" => $group , "acct_id" => $CGI[ acct_id ] ) );
								$record->data[ group_name ] = $group;
								$record->data[ acct_id ]    = $CGI[ acct_id ];
								$record->update_values ( $record->data );
								$record->save ();
								$report .= " - <i>$group</i><br />";
						}
						$output = "
               <b>$CGI[acct_id]</b><br /><br />
               $report
               Saved.
            ";
						admin_output ( "$language[edit_group_title] <b>$CGI[acct_id]</b>" , $output );
				}



				// --------------------------------------------------------------------- //
				//  Edit a group (lists out all accounts)
				// --------------------------------------------------------------------- //
				function edit_group ()
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						$users = '';
						$SQL   = "SELECT acct_id FROM " . ACCOUNTS_TABLE . " ORDER BY acct_id";
						$accts = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
						$SQL   = "SELECT acct_id,1 FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name = '$CGI[group]'";
						$glist = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'fullhash' ) );
						foreach ( $glist as $user => $exists ) {
								/* Modified by Akhilesh on 1/12/2011. htmlspecialchars () funciton added*/
								$users .= " <option value='" . htmlspecialchars ( $user ) . "' selected>" . htmlspecialchars ( $user ) . "</option> ";
						}
						foreach ( $accts as $user ) {
								if ( $glist[ $user ] ) {
										continue;
								}
								$users .= " <option value='" . htmlspecialchars ( $user ) . "'>" . htmlspecialchars ( $user ) . "</option> ";
						}
						$output = "

                <form action='$SCRIPT_NAME' method='post'>
                  <table width=500>
                   <tr>
                    <td colspan=2>
                      $language[manage_group_header]
                      <br /><br />
                    </td>
                   </tr>

                   <tr>
                    <td valign='top'>
                      <select name='users[]' multiple size=10>
                       $users
                      </select>
                    </td>

                    <td valign='top'>
                      $language[manage_group_add]<br />
                      <textarea name='new_users' cols=25 rows=10></textarea>
                    </td>
                  </tr>

                  <tr>
                    <td colspan=2>
                       $language[group_edit_name] <input type='text' name='group' value='$CGI[group]'>
                      <input type='hidden' name='original_group' value='$CGI[group]'>
                      <input type='hidden' name='action' value='save_group'>
                      <input type='hidden' name='admin' value='accounts'>
                      <input type='submit' value='$language[save_group_button]'>
                    </td>
                  </tr>
                </form>

           ";
						admin_output ( "$language[edit_group_title] <b>$CGI[group]</b>" , $output );
				}

				// --------------------------------------------------------------------- //
				//  Save assignments to a group
				// --------------------------------------------------------------------- //
				function save_group ()
				{
						global $CGI;
						global $language;
						global $iSQL;
						$output = '';
						# Rename Group, if necessary
						if ( $CGI[ group ] != $CGI[ original_group ] ) {
								$SQL = "UPDATE " . USER_GROUPS_TABLE . " SET group_name = '$CGI[group]' WHERE group_name = '$CGI[original_group]'";
								$iSQL->run ( $SQL );
								$SQL = "UPDATE " . GROUP_ASSIGNMENTS_TABLE . " SET group_name = '$CGI[group]' WHERE group_name = '$CGI[original_group]'";
								$iSQL->run ( $SQL );
						}
						## This will auto-create it if it's not already there.....
						$record                     = new Record( array ( "Parent" => $iSQL , "Table" => USER_GROUPS_TABLE , "Schema" => USER_GROUPS_TABLE , "Lookup" => 1 , "group_name" => $CGI[ group ] ) );
						$record->data[ group_name ] = $CGI[ group ];
						$record->update_values ( $record->data );
						$record->save ();
						## Nuke, and start over.
						$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name = '$CGI[group]'";
						$iSQL->run ( $SQL );
						$users     = $CGI[ users ];
						$new_users = preg_split ( "/\n/" , $CGI[ new_users ] );
						foreach ( $new_users as $user ) {
								$user = preg_replace ( "/(\cM|\r|\n)/" , "" , $user );
								$user = preg_replace ( "/^\s+/" , "" , $user );
								$user = preg_replace ( "/\s$/" , "" , $user );
								if ( $user ) {
										array_push ( $users , $user );
								}
						}
						foreach ( $users as $user ) {
								$record                     = new Record( array ( "Parent" => $iSQL , "Table" => GROUP_ASSIGNMENTS_TABLE , "Schema" => GROUP_ASSIGNMENTS_TABLE ) );
								$record->data[ group_name ] = $CGI[ group ];
								$record->data[ acct_id ]    = $user;
								$record->update_values ( $record->data );
								$record->save ();
								$output .= " - <i>$user</i><br />";
						}
						admin_output ( "$language[edit_group_title] <b>$CGI[group]</b>" , "Operation Complete<br /><br />Saved Group: $CGI[group]<br /><br />Members: <br />$output" );
				}


				// --------------------------------------------------------------------- //
				//  Delete a group
				// --------------------------------------------------------------------- //
				function delete_group ()
				{
						global $CGI;
						global $language;
						global $iSQL;
						## Nuke, and start over.
						$SQL = "DELETE FROM " . USER_GROUPS_TABLE . " WHERE group_name = '$CGI[group]'";
						$iSQL->run ( $SQL );
						$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name = '$CGI[group]'";
						$iSQL->run ( $SQL );
						admin_output ( "$language[delete_group_title] <b>$CGI[group]</b>" , "Group Deleted" );
				}




				# ----------------------------------------------------------------------------------------- #
				#  Money and Financials
				# ----------------------------------------------------------------------------------------- #
				// --------------------------------------------------------------------- //
				//  Quick list of all deposit activity for an account
				// --------------------------------------------------------------------- //
				function deposit_history ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						if ( $CGI[ 'start' ] ) {
								$start = $CGI[ 'start' ];
						} else {
								$start = 0;
						}
						$end     = $start + 9;
						$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						list ( $history , $count ) = $account->deposit_list ( array ( "start" => $start , "end" => $end ) );
						$nav = '';
						if ( $start > 0 ) {
								$prev = $start - 9;
								$nav  = " <a href='$SCRIPT_NAME?admin=accounts&action=deposit_history&acct_id=$CGI[acct_id]&start=$prev'>$language[previous_link]</a> ";
						}
						if ( $count > $end ) {
								$next = $start + 9;
								$nav  = " <a href='$PROG_URL?admin=accounts&action=deposit_history&acct_id=$CGI[acct_id]&start=$next'>$language[next_link]</a> ";
						}
						$OUTPUT = "
                <table>
                  $history
                </table>
                <br /><br />
                $nav
            ";
						admin_output ( $language[ deposits_title ] , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  More detailed deposit listing
				// --------------------------------------------------------------------- //
				function deposit_summary ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						if ( $CGI[ 'start' ] ) {
								$start = $CGI[ 'start' ];
						} else {
								$start = 0;
						}
						$end   = $start + 9;
						$WHERE = '';
						if ( $CGI[ type ] ) {
								$WHERE = "WHERE trans_type = '$CGI[type]'";
						}
						$SQL     = "SELECT * FROM " . TRANSACTIONS_TABLE . " $WHERE ORDER BY trans_date DESC";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$counter = -1;
						$output  = "
              <table border='0' cellspacing='1' cellpadding='5' bgcolor='#e0e0e0'>
                <tr bgcolor='silver'>
                   <th>Acct</th>
                   <th>Date</th>
                   <th>Amount</th>
                   <th>Source</th>
                </tr>
           ";
						foreach ( $results as $deposit ) {
								$counter++;
								if ( $counter < $start ) {
										continue;
								}
								$date   = date ( $language[ date_format ] , $deposit[ trans_date ] );
								$source = $deposit[ source ];
								if ( $deposit[ extra_info ] ) {
										$source = " <a href='$SCRIPT_NAME?admin=accounts&action=finance_detail&date=$deposit[trans_date]&acct_id=$deposit[acct_id]'>$deposit[source]</a> ";
								}
								$deposit[ amount ] = sprintf ( "%5.2f" , $deposit[ trans_amount ] );
								$output .= "
                 <tr bgcolor='white'>
                   <td>$deposit[acct_id]</td>
                   <td>$date</td>
                   <td align='right' nowrap><b>$language[dollar_sign] $deposit[trans_amount]</b></td>
                   <td><i>$source</i></td>
                 </tr>
               ";
								if ( $counter >= $end ) {
										break;
								}
						}
						$output .= "</table>";
						$count = count ( $results );
						$nav   = '';
						if ( $start > 0 ) {
								$prev = $start - 9;
								$nav  = " <a href='$SCRIPT_NAME?admin=accounts&action=deposit_summary&type=$CGI[type]&start=$prev'>$language[previous_link]</a> ";
						}
						if ( $count > $end ) {
								$next = $start + 9;
								$nav  = " <a href='$SCRIPT_NAME?admin=accounts&action=deposit_summary&type=$CGI[type]&start=$next'>$language[next_link]</a> ";
						}
						admin_output ( $language[ deposits_title ] , $output . $nav );
				}


				// --------------------------------------------------------------------- //
				//  View a single transaction
				// --------------------------------------------------------------------- //
				function finance_detail ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						$detail  = $account->financial_details ( $CGI[ 'date' ] );
						admin_output ( $language[ deposits_title ] , $detail );
				}


				// --------------------------------------------------------------------- //
				//  List out pending deposits
				// --------------------------------------------------------------------- //
				function deposits ( $message = '' )
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						$SQL      = "SELECT * FROM " . DEPOSITS_TABLE;
						$deposits = $oSQL->search ( array ( "SQL" => $SQL ) );
						$report   = "";
						foreach ( $deposits as $deposit ) {
								$report .= "
                    <tr>
                        <td><b>$deposit[acct_id]</b></td>
                        <td>  \$  $deposit[amount]</td>
                        <td>
                           <a href='$SCRIPT_NAME?admin=accounts&action=apply_deposit&acct_id=$deposit[acct_id]&amount=$deposit[amount]'>$language[accept_deposit_link]</a>
                           <a href='$SCRIPT_NAME?admin=accounts&action=cancel_deposit&acct_id=$deposit[acct_id]'>$language[cancel_deposit_link]</a>
                        </td>
                    </tr>
              ";
						}
						$OUTPUT = "
              <br />
              <b>$message</b>
              <blockquote>
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>
                <table cellspacing='0' onclick='sortcolumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td style='width: 100px;'>$language[deposits_account]</td>
                        <td style='width: 100px;'>$language[deposits_amount]</td>
                        <td style='width: 160px;'>$language[deposits_action]</td>
                    </tr>
                </thead>
                <tbody>
                   $report
                </tbody>
                </table>
              </blockquote>
            ";
						admin_output ( $language[ deposits_title ] , $OUTPUT );
				}

				// --------------------------------------------------------------------- //
				//  Delete a pending deposit
				// --------------------------------------------------------------------- //
				function cancel_deposit ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						$id  = $CGI[ acct_id ];
						$SQL = "DELETE FROM " . DEPOSITS_TABLE . " WHERE acct_id = '$id'";
						$iSQL->run ( $SQL );
						admin_output ( $language[ deposits_title ] , $language[ 'deposits_cancel_ok' ] );
				}


				// --------------------------------------------------------------------- //
				//  Manually accept a pending deposit
				// --------------------------------------------------------------------- //
				function apply_deposit ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $templates;
						$account = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						// Setup some $CGI vars so that handle_deposit works
						$CGI[ source ] = "Admin Approval";
						$message       = $account->handle_deposit ();
						// Send out an email //
						$updatedaccount     = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						$values             = array ();
						$values[ login_id ] = $updatedaccount->data[ acct_id ];
						$values[ password ] = $updatedaccount->data[ acct_password ];
						$values[ balance ]  = $updatedaccount->data[ current_balance ];
						$values[ amount ]   = $CGI[ amount ];
						$values             = array_merge ( $acct_info , $values );
						// Format the amounts to look consistent
						$values[ current_balance ] = sprintf ( "%.2f" , $values[ current_balance ] );
						$values[ balance ]         = sprintf ( "%.2f" , $values[ balance ] );
						$values[ amount ]          = sprintf ( "%.2f" , $values[ amount ] );
						$values[ tax ]             = sprintf ( "%.2f" , $values[ tax ] );
						$values[ total ]           = sprintf ( "%.2f" , $values[ total ] );
						// Make the server date available
						$values[ serverdate ] = date ( "Y-m-d" );
						$ttxt                 = $templates[ 'acct_admin' ][ 'default' ][ 'EMAIL' ][ $config[ jh_user_payment_email ] ];
						$template             = new SubTemplate( array ( "front" => "<<" , "back" => ">>" , "template_text" => $ttxt , "values" => $values ) );
						$email_text           = $template->merge ();
						$to                   = $account->data[ contact_email ];
						$subject              = $config[ jh_user_payment_email ];
						mail ( $to , $subject , $email_text , "From: $config[email]\r\n" );
						$this->deposits ( $message );
				}


				// --------------------------------------------------------------------- //
				//  Add/Subtract money for a single account
				// --------------------------------------------------------------------- //
				function adjust_balance ( $message = '' )
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						$OUTPUT = "
              <table border=0 width=400>
                  <tr>
                     <td>
                          <font color='red'>$message</font><br /><br />
                          <b>
                             $language[adjust_header] $CGI[acct_id]<br />
                          </b>
                          <form action='$SCRIPT_NAME' method='post'>
                            $language[adjust_acct_prompt] <input type='text' name='acct_id' value='$CGI[acct_id]'><br />
                            $language[adjust_amt_prompt]<input name='amount' value='$CGI[amount]'><br />
                            $language[adjust_amt_howto]<br /><br />

                            Source: <input name='comment' maxlength=50><br /><br />
                            Additional Notes:<br />
                            <textarea name='extra_info' rows=5 cols=50>$CGI[extra_info]</textarea><br /><br />
                            <input type='hidden' name='action' value='apply_adjusted_balance'>
                            <input type='hidden' name='affiliate' value='$CGI[affiliate]'>
                            <input type='hidden' name='lpd' value='$CGI[lpd]'>
                            <input type='hidden' name='admin' value='accounts'>
                            <input type='submit' value='$language[adjust_button]'>

                          </form>
                     </td>
                  </tr>
              </table>
            ";
						admin_output ( $language[ adjust_title ] , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  Save the adjusted balance
				// --------------------------------------------------------------------- //
				function apply_adjusted_balance ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						$reactivate = '';
						$report     = '';
						$account    = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						$balance    = $account->data[ current_balance ];
						if ( !$account->data[ acct_id ] || ( $CGI[ acct_id ] != $account->data[ acct_id ] ) ) {
								$this->adjust_balance ( "<b>$CGI[acct_id]:</b> $language[invalid_account]" );
								exit;
						}
						## Update account totals
						$account->data[ current_balance ] += $CGI[ amount ];
						$account->data[ total_paid ] += $CGI[ amount ];
						$account->data[ last_pd_date ] = time ();
						if ( $account->update_values ( $account->data ) ) {
								if ( $account->save () ) {
										$report     = "$language[adjust_ok]<br />";
										$reactivate = $account->reactivate_bids ();
										## New finance Table Record
										$account->financial_entry ( array ( "deposit_type" => "ADJUST" , "amount" => $CGI[ amount ] , "source" => $CGI[ comment ] , "extra_info" => $CGI[ extra_info ] ) );
								} else {
										$report = "$language[adjust_error]<pre>$account->error_text</pre>";
								}
						} else {
								$report = "$language[adjust_error]<pre>$account->error_text</pre>";
						}
						if ( $CGI[ affiliate ] ) {
								$SQL     = "SELECT balance FROM " . AFF_PAYMENTS_TABLE . " WHERE affiliate = '$CGI[acct_id]' ORDER BY tstamp DESC LIMIT 1";
								$balance = $iSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
								## We're marking clicks as paid during the adjustment process, but haven't actually run them through
								## the payment report to have this amount added to the amount to actually pay the person
								## so, add this to their carryover balace so the next payment report picks it up.
								$balance                      = $balance + $CGI[ amount ];
								$payment                      = new Record( array ( 'Parent' => $iSQL , 'Table' => AFF_PAYMENTS_TABLE , 'Schema' => AFF_PAYMENTS_TABLE ) );
								$payment->data[ affiliate ]   = $CGI[ acct_id ];
								$payment->data[ paid_date ]   = date ( "Ymd" );
								$payment->data[ paid_amount ] = $CGI[ amount ];
								$payment->data[ balance ]     = $balance;
								$payment->data[ tstamp ]      = time ();
								$payment->data[ comments ]    = $CGI[ extra_info ];
								$payment->update_values ( $payment->data );
								$payment->save ();
								## Now, mark all those clicks as paid
								$SQL = "UPDATE " . CLICK_LOG_TABLE . " SET paid_flag = 'YES' WHERE hit_date <= '$CGI[lpd]' AND affiliate = '$CGI[acct_id]'";
								$iSQL->run ( $SQL );
						}
						$cbal   = $account->data[ current_balance ];
						$OUTPUT = "
              <table border=0 width=400>
                  <tr>
                      <td>
                      <b>
                         $language[adjust_summary_account] $CGI[acct_id]<br />
                         $language[adjust_summary_previous] $language[dollar_sign] $balance<br />
                         $language[adjust_summary_current] $language[dollar_sign] $cbal
                      </b>
                      <br />$language[adjust_summary_header]<br />
                      $report
                      <pre>
                      $reactivate
                      </pre>
                      </td>
                  </tr>
              </table>
           ";
						admin_output ( $language[ adjust_title ] , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  List pending withdrawls
				// --------------------------------------------------------------------- //
				function withdrawls ( $message = '' )
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$SQL        = "SELECT * FROM " . WITHDRAWLS_TABLE;
						$withdrawls = $oSQL->search ( array ( "SQL" => $SQL ) );
						$report     = '';
						foreach ( $withdrawls as $withdrawl ) {
								$report .= "
                    <tr>
                        <td><b>$withdrawl[acct_id]</b></td>
                        <td>  \$  $withdrawl[withdrawl_amount]</td>
                        <td>  $withdrawl[payment_method]</td>
                        <td>
                           <a href='$SCRIPT_NAME?admin=accounts&action=show_withdrawl&acct_id=$withdrawl[acct_id]&amount=$withdrawl[withdrawl_amount]'>$language[do_withdrawl_link]</a>
                        </td>
                    </tr>
              ";
						}
						$OUTPUT = "
              <br />
              <B>$message</B>
              <blockquote>

               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

                <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td style='width: 100px;'>$language[withdrawl_account]</td>
                        <td style='width: 60px;'>$language[withdrawl_amount]</td>
                        <td style='width: 100px;'>$language[withdrawl_method]</td>
                        <td style='width: 100px;'>$language[do_withdrawl_link]</td>
                    </tr>
                </thead>
                <tbody>
                   $report
                </tbody>
                </table>

              </blockquote>
            ";
						admin_output ( $language[ withdrawl_title ] , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  View a single withdrawl
				// --------------------------------------------------------------------- //
				function show_withdrawl ()
				{
						global $CGI;
						global $oSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$account   = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						$withdrawl = new Record( array ( "Parent" => $oSQL , "Table" => WITHDRAWLS_TABLE , "Schema" => WITHDRAWLS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] , "withdrawl_amount" => $CGI[ amount ] ) );
						$id        = $withdrawl->data[ acct_id ];
						$amount    = $withdrawl->data[ withdrawl_amount ];
						$method    = $withdrawl->data[ payment_method ];
						if ( $id && $amount ) {
								$SQL     = "SELECT * FROM " . ACCOUNTS_TABLE . " WHERE acct_id = '$id'";
								$acct    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'hash' ) );
								$subject = urlencode ( $language[ withdrawl_paypal_subject ] );
								$email   = $account->data[ contact_email ];
								$href    = "https://www.paypal.com/xclick/business=$email&item_name=$subject&item_number=1&amount=$amount&no_shipping=1";
								$link    = " <a href='$href' target='new'>$language[withdrawl_paypal_link]</a> ";
								$OUTPUT  = "
                  <br />
                    <table width=500 bgcolor='black' cellspacing=1 cellpadding=0 border=0>
                     <tr>
                       <td>
                         <table width=100% bgcolor='#e0e0e0' cellspacing=0 cellpadding=8>
                           <tr bgcolor='#596e88'>
                             <td colspan=2>
                               <font color='white'>$language[withdrawl_title] <b>$id</b></font>
                             </td>
                           </tr>
                           <tr>
                             <td valign='top'>
                                 $language[withdrawl_account] <b>$id</b><br />
                                 $language[withdrawl_amount] <b>$amount</b><br />
                                 $language[withdrawl_method] <b>$method</b><br /><br />
                                 $language[header_balance] <b>$acct[current_balance]</b>
                             </td>
                             <td valign='top'>
                                 <b>
                                 $acct[contact_name]<br />
                                 $acct[address]<br />
                                 $acct[address2]<br />
                                 $acct[city], $acct[state] $acct[zip]<br />
                                 $acct[phone] / $acct[fax]<br />
                                 $acct[country]
                                 </b>
                             </td>
                           </tr>

                           <tr bgcolor='silver'>
                             <td>
                               &nbsp; $link
                             </td>
                             <td>
                               <a href='$SCRIPT_NAME?admin=accounts&action=complete_withdrawl&acct_id=$id&amount=$amount'>$language[withdrawl_mark_done_link]</a> /
                               <a href='$SCRIPT_NAME?admin=accounts&action=cancel_withdrawl&acct_id=$id&amount=$amount'>$language[withdrawl_cancel_link]</a>
                             </td>
                           </tr>
                         </table>
                         <center><a href='javascript:window.print()'><font color='white'>$language[print_link]</font></a></center>
                       </td>
                     </tr>
                   </table>

                ";
						} else {
								$OUTPUT = $language[ withdrawl_invalid ];
						}
						admin_output ( $language[ withdrawl_title ] , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  Perform the withdrawl (remove it from pending, give link to pay)
				// --------------------------------------------------------------------- //
				function complete_withdrawl ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$message   = '';
						$account   = new Account( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
						$withdrawl = new Record( array ( "Parent" => $iSQL , "Table" => WITHDRAWLS_TABLE , "Schema" => WITHDRAWLS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] , "withdrawl_amount" => $CGI[ amount ] ) );
						$id        = $withdrawl->data[ acct_id ];
						$amount    = $withdrawl->data[ withdrawl_amount ];
						$method    = $withdrawl->data[ payment_method ];
						if ( $id && $amount ) {
								$bal = $account->data[ current_balance ] - $amount;
								if ( $bal >= 0.00 ) {
										$account->data[ current_balance ] = $bal;
										$account->update_values ( $account->data );
										$account->save ();
										$account->financial_entry ( array ( "deposit_type" => "WITHDRAWL" , "amount" => "-" . $CGI[ amount ] , "source" => $method ) );
										$withdrawl->delete ();
										$message = "Withdrawl Complete";
								} else {
										$message = $language[ withdrawl_fail_balance ];
								}
						} else {
								$message = $language[ withdrawl_invalid ];
						}
						admin_output ( $language[ withdrawl_title ] , $message );
				}


				// --------------------------------------------------------------------- //
				//  Cancel pending withdrawl request
				// --------------------------------------------------------------------- //
				function cancel_withdrawl ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$withdrawl = new Record( array ( "Parent" => $iSQL , "Table" => WITHDRAWLS_TABLE , "Schema" => WITHDRAWLS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] , "withdrawl_amount" => $CGI[ amount ] ) );
						$withdrawl->delete ();
						admin_output ( $language[ withdrawl_title ] , "Withdrawl: $CGI[acct_id] ... $language[dollar_sigh] $CGI[amount] Cancelled" );
				}


				// --------------------------------------------------------------------- //
				//  List support tickets of the desired type (open or closed)
				// --------------------------------------------------------------------- //
				function list_tickets ()
				{
						global $CGI;
						global $oSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$header      = '';
						$list        = '';
						$ticket_list = '';
						if ( !$CGI[ type ] ) {
								$CGI[ type ] = "O";
						}
						if ( $CGI[ type ] == "O" ) {
								$header = $language[ open_tickets_header ];
						} elseif ( $CGI[ type ] == "C" ) {
								$header = $language[ closed_tickets_header ];
						}
						$SQL     = "SELECT * FROM " . TICKETS_TABLE . " WHERE ticket_status = '$CGI[type]'";
						$tickets = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $tickets as $ticket ) {
								$date  = formatdate ( $ticket[ ticket_date ] );
								$style = "style='color:red;'";
								if ( $ticket[ read_by_agent ] >= 1 ) {
										$style = '';
								}
								$ticket_list .= "
                   &#183; &nbsp;
                   <a $style href='$SCRIPT_NAME?admin=accounts&action=view_ticket&ticket_id=$ticket[ticket_id]'>$ticket[subject]</a> [$date]<br />
                 ";
						}
						$OUTPUT = "
                $language[list_tickets]
                <a href='$SCRIPT_NAME?admin=accounts&action=list_tickets&type=O'>$language[open_tickets_link]</a>
                &#183;
                <a href='$SCRIPT_NAME?admin=accounts&action=list_tickets&type=C'>$language[closed_tickets_link]</a>
                <div style='border-bottom:1px solid black'>&nbsp;</div>
                <br />
                <b>$header</b>
                <div style='font-size:8pt'>
                $ticket_list
                </div>
            ";
						admin_output ( "Support Tickets" , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  View a single support ticket
				// --------------------------------------------------------------------- //
				function view_ticket ( $message = '' )
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$form = '';
						$SQL  = "UPDATE " . TICKETS_TABLE . " SET read_by_agent = 1 WHERE ticket_id = $CGI[ticket_id]";
						$iSQL->run ( $SQL );
						$ticket                     = new Record( array ( "Parent" => $iSQL , "Table" => TICKET_REPLIES_TABLE , "Schema" => "support_ticket_replies_admin" ) );
						$ticket->data[ ticket_id ]  = $CGI[ ticket_id ];
						$ticket->data[ reply_date ] = date ( "Ymd" );
						$ticket->update_values ();
						$formarray  = $ticket->generate_form ();
						$formfields = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$form .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$SQL            = "SELECT * FROM " . TICKETS_TABLE . " WHERE ticket_id = $CGI[ticket_id]";
						$ticket_info    = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
						$SQL            = "SELECT * FROM " . TICKET_REPLIES_TABLE . " WHERE ticket_id = $CGI[ticket_id] ORDER BY reply_date ASC";
						$replies        = $iSQL->search ( array ( "SQL" => $SQL ) );
						$date           = formatdate ( $ticket_info[ ticket_date ] );
						$ticket_history = "
                  <div style='font-size:8pt'>
                  <b>Ticket ID: </b> $ticket_info[ticket_id]<br />
                  <b>Submitted By: </b> $ticket_info[acct_id]<br />
                  <b>On: </b> $date<br />
                  <b>Current Status: </b> $ticket_info[ticket_status]<br />
                  <b>Assigned To: </b> $ticket_info[assigned_agent]<br />
                  <br />
                  <b>$ticket_info[subject]</b>
                  <br />
                  <div style='padding:5px;padding-left:10px'>$ticket_info[message]</div>
                  <div style='border-bottom:1px solid black'>&nbsp;</div>
             ";
						if ( $replies ) {
								foreach ( $replies as $reply ) {
										$date             = formatdate ( $reply[ reply_date ] );
										$reply[ message ] = preg_replace ( "/(\cM|\n|\r)/" , "<br />" , $reply[ message ] );
										$ticket_history .= "
                         <b>Reply From: $reply[reply_by] on $date</b>
                         <br />
                         <div style='padding:5px;padding-left:10px'>$reply[message]</div>
                         <div style='border-bottom:1px solid black'>&nbsp;</div>
                      ";
								}
						}
						$ticket_history .= "</div>";
						$open_close_link = " <b><a href='$SCRIPT_NAME?admin=accounts&action=close_ticket&ticket_id=$CGI[ticket_id]'>Close This Ticket</a></b> ";
						if ( $ticket_info[ ticket_status ] == "C" ) {
								$open_close_link = " <b><a href='$SCRIPT_NAME?admin=accounts&action=open_ticket&ticket_id=$CGI[ticket_id]'>Re-Open This Ticket</a></b> ";
						}
						$OUTPUT = "
                $message

                $language[list_tickets]
                <a href='$SCRIPT_NAME?admin=accounts&action=list_tickets&type=O'>$language[open_tickets_link]</a>
                &#183;
                <a href='$SCRIPT_NAME?admin=accounts&action=list_tickets&type=C'>$language[closed_tickets_link]</a>
                <div style='border-bottom:1px solid black'>&nbsp;</div>
                <br />

                <table cellspacing='10'>
                  <tr>
                     <td valign='top' align='left' width='10%' nowrap='nowrap'>

                        $open_close_link
                        <br /><br />

                        <b>$language[submit_reply]</b>
                        <form name='supportform' action='$SCRIPT_NAME' method='post'>
                            <table>
                                $form
                                <tr><td colspan=2 align='center'><input type='submit' value='$language[submit_reply]'></td></tr>
                            </table>
                            <input type='hidden' name='action' value='ticket_reply'>
                            <input type='hidden' name='admin' value='accounts'>
                        </form>

                     </td>
                     <td style='background:#eeeeee; border:1px solid black; padding:5px;' valign='top' align='left' width='100%'>
                        $ticket_history
                     </td>
                  </tr>
                </table>
             ";
						admin_output ( "Support Tickets" , $OUTPUT );
				}


				// --------------------------------------------------------------------- //
				//  Add a reply to a ticket
				// --------------------------------------------------------------------- //
				function ticket_reply ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $templates;
						$SQL = "UPDATE " . TICKETS_TABLE . " SET read_by_user = 0 WHERE ticket_id = $CGI[ticket_id]";
						$iSQL->run ( $SQL );
						$ticket            = new Record( array ( "Parent" => $iSQL , "Table" => TICKET_REPLIES_TABLE , "Schema" => TICKET_REPLIES_TABLE ) );
						$CGI[ reply_date ] = date ( "Ymd" );
						$ticket->update_values ( $CGI );
						$ticket->save ();
						// Table Names
						$accounts = ACCOUNTS_TABLE;
						$tickets  = TICKETS_TABLE;
						## Send email to the user ##
						$SQL         = "SELECT contact_email FROM $accounts, $tickets WHERE $tickets.ticket_id = $CGI[ticket_id] AND $tickets.acct_id = $accounts.acct_id";
						$to          = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$ttxt        = $templates[ 'acct_admin' ][ 'default' ][ 'EMAIL' ][ $config[ jh_ticket_reply_email ] ];
						$toptemplate = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $ttxt , "values" => $CGI ) );
						$body        = $toptemplate->merge ();
						mail ( $to , $config[ jh_ticket_reply_email ] , $body , "From: $config[email]\r\n" );
						$this->view_ticket ();
				}


				// --------------------------------------------------------------------- //
				//  Mark a ticket as closed
				// --------------------------------------------------------------------- //
				function close_ticket ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $templates;
						$ticket               = new Record( array ( "Parent" => $iSQL , "Table" => TICKETS_TABLE , "Schema" => TICKETS_TABLE , "Lookup" => 1 , "ticket_id" => $CGI[ ticket_id ] ) );
						$CGI[ ticket_status ] = "C";
						$ticket->update_values ( $CGI );
						$ticket->save ();
						$this->view_ticket ( "<span style='color:red'><b>Ticket Closed</b></span><br /><br />" );
				}


				// --------------------------------------------------------------------- //
				//  RE-Open a closed ticket
				// --------------------------------------------------------------------- //
				function open_ticket ()
				{
						global $CGI;
						global $iSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						global $templates;
						$ticket               = new Record( array ( "Parent" => $iSQL , "Table" => TICKETS_TABLE , "Schema" => TICKETS_TABLE , "Lookup" => 1 , "ticket_id" => $CGI[ ticket_id ] ) );
						$CGI[ ticket_status ] = "O";
						$ticket->update_values ( $CGI );
						$ticket->save ();
						$this->view_ticket ( "<span style='color:red'><b>Ticket Re-Opened</b></span><br /><br />" );
				}




				// --------------------------------------------------------------------------------
				//      AFFILIATE FUNCTIONS
				// --------------------------------------------------------------------------------
				function affiliate_report_form ()
				{
						$report_form = jackhammer_report_form ( "affiliates" );
						admin_output ( $language[ aff_report_title ] , $report_form );
				}

				function list_pending_affiliates ()
				{
						global $oSQL;
						$SQL     = "SELECT acct_id FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name LIKE 'Pending%'";
						$pending = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
						$list    = join ( "','" , $pending );
						// Tables
						$accounts = ACCOUNTS_TABLE;
						$affinfo  = AFF_INFO_TABLE;
						$SQL      = "
                SELECT $accounts.*, $affinfo.*
                FROM $accounts, $affinfo
                WHERE $accounts.acct_id IN('$list')
                AND $affinfo.affiliate = $accounts.acct_id
            ";
						$this->list_affiliates ( "affiliate_review" , $SQL , 1 );
				}

				function affiliate_review ()
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						global $templates;
						$step    = $CGI[ step ];
						$letters = $templates[ 'jackhammer_admin' ][ 'default' ][ 'EMAIL' ];
						foreach ( $letters as $name => $text ) {
								$emails .= " <option value='$name'>$name</option> ";
						}
						$account          = new Record( array ( "Parent" => $oSQL , "Lookup" => 1 , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "acct_id" => $CGI[ affiliate ] ) );
						$affiliate        = new Record( array ( "Parent" => $oSQL , "Lookup" => 1 , "Table" => AFF_INFO_TABLE , "Schema" => AFF_INFO_TABLE , "affiliate" => $CGI[ affiliate ] ) );
						$account_fields   = $account->generate_form ();
						$affiliate_fields = $affiliate->generate_form ();
						$form             = '';
						while ( list ( $prompt , $fieldinput ) = each ( $account_fields ) ) {
								$form .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$form .= " <tr bgcolor='white'><td colspan=2><b>$language[affiliate_review_header]</b></td></tr> ";
						while ( list ( $prompt , $fieldinput ) = each ( $affiliate_fields ) ) {
								$form .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$OUTPUT = "
              <table border=0><tr><td>

                <br />
                $language[affiliate_review_explain]
                <p>
                  <form name='aform' action='$SCRIPT_NAME' method='post'>
                  <table cellpadding=8 cellspacing=0 border=0>
                    <tr bgcolor='navy'><td colspan=2>
                      <font color='yellow'>
                         $language[affiliate_review_account] <b>$cgi[affiliate]</b>
                      </font>
                    </td></tr>
                    <tr bgcolor='white'><td colspan=2><b>$language[affiliate_review_acct_info]</td></tr>

                    $form

                    <tr bgcolor='white'><td colspan=2><b>$language[affiliate_review_options]</td></tr>
                    <tr>
                       <td><b>action:</b></td>
                       <td>
                          <select name='status'>
                          <option value='approve'>$language[affiliate_review_approve]</option>
                          <option value='deny'>$language[affiliate_review_deny]</option>
                          <option value='delete'>$language[affiliate_review_deny_del]</option>
                          <option value='nuke'>$language[affiliate_review_nuke]</option>
                          </select>
                        </td>
                     </tr>

                    <tr>
                       <td><b>$language[affiliate_review_email_prompt]</b></td>
                       <td>
                          <select name='email'>
                             <option value=''>$language[affiliate_review_no_email]</option>
                             $emails
                          </select>
                        </td>
                     </tr>

                    <tr>
                      <td colspan=2 align='center'>
                         <input type='hidden' name='action' value='process_affiliate'>
                         <input type='hidden' name='admin' value='accounts'>
                         <input type='submit' value='$language[affiliate_review_button]'>
                      </td>
                    </tr>
                   </table>
                 </form>

              </td></tr></table><br />

              <script language='javascript'>
                  function selectcats() {
                      cats=window.open ('$SCRIPT_NAME?admin=accounts&action=aff_categories','X','toolbar=no,scrollbars=yes,status=yes,directories=no,resizable=yes,menubar=no,width=625,height=500')
                  }
              </script>

            ";
						admin_output ( "$language[affiliate_review_title] $CGI[affiliate]" , $OUTPUT );
				}

				function process_affiliate ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						global $tempaltes;
						## not in the form, so it has to be manually set.
						$CGI[ affiliate ] = $CGI[ acct_id ];
						## create the objects.
						$account   = new Account( array ( "Parent" => $iSQL , "Lookup" => 1 , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "acct_id" => $CGI[ affiliate ] ) );
						$affiliate = new Record( array ( "Parent" => $iSQL , "Lookup" => 1 , "Table" => AFF_INFO_TABLE , "Schema" => AFF_INFO_TABLE , "affiliate" => $CGI[ affiliate ] ) );
						$message   = "<b><U>$language[proc_review_summary]</U></b><br /><br />\n";
						## Need to handle the approval of affiliates, by sending an email (or not),
						## creating a brand (or not), and saving the edits made by the administrator.
						if ( $CGI[ status ] == "approve" ) {
								$account->update_values ( $CGI );
								if ( !$CGI[ 'search_amount' ] ) {
										$CGI[ 'search_amount' ] = $config[ 'jackhammer_affiliate_search_payment' ];
								}
								if ( !$CGI[ 'click_amount' ] ) {
										$CGI[ 'click_amount' ] = $config[ 'jackhammer_affiliate_click_payment' ];
								}
								if ( !$CGI[ 'revshare_amount' ] ) {
										$CGI[ 'revshare_amount' ] = $config[ 'jackhammer_affiliate_revshare_payment' ];
								}
								if ( $account->save () ) {
										$message .= "$language[proc_review_accountt_ok]<br />";
								} else {
										die( "$language[proc_review_account_error] <pre>$account->error_text</pre>" );
								}
								$affiliate->update_values ( $CGI );
								if ( $affiliate->save () ) {
										$message .= "$language[proc_review_aff_ok]<br />";
								} else {
										die( "$language[proc_review_aff_error] .. <pre>$affiliate->error_text</pre>" );
								}
								$pSQL = "UPDATE " . GROUP_ASSIGNMENTS_TABLE . " SET group_name = 'Affiliate' WHERE group_name = 'Pending Affiliate' AND acct_id = '$CGI[affiliate]'";
								$iSQL->run ( $pSQL );
								$message .= "<br />";
						} elseif ( $CGI[ status ] == "deny" ) {
								$message .= "$language[proc_review_denied]<br />";
								$message .= $affiliate->delete () . "<br /><br />";
								$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE acct_id = '$CGI[affiliate]'";
								$iSQL->run ( $SQL );
						} elseif ( $CGI[ status ] == "delete" ) {
								$message .= "$language[proc_review_deny_del]<br /><br />";
								$message .= $language[ proc_review_account ] . $account->delete () . "<br />";
								$message .= $language[ proc_review_aff ] . $affiliate->delete () . "<br /><br />";
								$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE acct_id = '$CGI[affiliate]'";
								$iSQL->run ( $SQL );
						} elseif ( $CGI[ status ] == "nuke" ) {
								include_once ( "dsX/iSQL/Record/Listing" );
								$message .= "$language[proc_review_nuke]<br /><br />";
								$message .= $language[ proc_review_account ] . $account->delete () . "<br />";
								$message .= $language[ proc_review_aff ] . $affiliate->delete () . "<br /><br />";
								$SQL = "DELETE FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE acct_id = '$CGI[affiliate]'";
								$iSQL->run ( $SQL );
								$links = $account->get_links ();
								foreach ( $links as $link ) {
										$listing = new Listing( array ( "Parent" => $iSQL , "Lookup" => 1 , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "id" => $link[ id ] ) );
										$listing->delete ();
								}
						}
						## Send Email ??
						if ( $CGI[ email ] != "" ) {
								$email    = $account->data[ contact_email ];
								$SQL      = "SELECT * FROM " . TEMPLATES_TABLE . " WHERE theme='default' AND program='jackhammer_admin' AND template_name = '$CGI[email]'";
								$letter   = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
								$template = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $letter[ template_text ] , "values" => $CGI ) );
								$body     = $template->merge ();
								mail ( $email , $letter[ template_name ] , $body , "From: $config[email]\r\n" );
								$message .= "$language[proc_review_email] $email <PRE>$body</PRE><br /><br />";
						}
						admin_output ( "$language[proc_review_title] $CGI[affiliate]" , $message );
				}

				function list_affiliates ( $action = "affiliate_edit" , $SQL = '' , $pending = 0 )
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						global $tempaltes;
						global $data_dir;
						$do_taxes   = 0;
						$tax_header = "<td></td>";
						if ( file_exists ( "$data_dir/custom/taxforms.inc" ) ) {
								$do_taxes   = 1;
								$tax_header = "<td>Tax Form</td>";
						}
						$search_form = "
                <form style='margin:0px' action='$SCRIPT_NAME' method='post' target='main'>
                     $language[list_affiliates_prompt] <br />
                     <input name='id' size=10>
                     <input name='action' type='hidden' value='list_affiliates'>
                     <input name='admin' type='hidden' value='accounts'>
                     <input type='submit' value='$language[list_affiliates_button]'>
                </form>
            ";
						$total       = 0;
						$start       = 0;
						$end         = 0;
						$page        = 0;
						// Tables
						$accounts = ACCOUNTS_TABLE;
						$affinfo  = AFF_INFO_TABLE;
						if ( !$CGI[ nl ] ) {
								if ( !$SQL ) {
										$SQL = "
                        SELECT $accounts.*, $affinfo.*
                        FROM $accounts, $affinfo
                        WHERE $affinfo.affiliate LIKE '%$CGI[id]%'
                        AND $affinfo.affiliate = $accounts.acct_id
                        ORDER BY $accounts.acct_id ASC
                    ";
								}
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								$program = "admanager";
								$total   = count ( $results );
								$page    = $CGI[ page ];
								if ( !$page ) {
										$page = 1;
								}
								$start = $page - 1;
								$start = $start * 50;
								if ( !$start ) {
										$start = $start = 1;
								}
								$end = $start + 49;
								if ( $end > $total ) {
										$end = $total;
								}
								$shown = 0;
								foreach ( $results as $account ) {
										$shown++;
										if ( $shown < $start ) {
												continue;
										}
										if ( $shown > $end ) {
												break;
										}
										$tax_form_link = '';
										if ( $do_taxes ) {
												$SQL    = "SELECT count(*) FROM iweb_affiliate_tax_forms WHERE affiliate = '$account[acct_id]'";
												$exists = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
												if ( $exists ) {
														$tax_form_link = "<a href='$SCRIPT_NAME?admin=accounts&action=print_tax_form&affiliate=$account[acct_id]'>View</a>";
												} else {
														$tax_form_link = "<i>Missing</i>";
												}
										}
										$list_ie .= "
                         <tr style='cursor:pointer' ondblclick='document.location.href=\"$SCRIPT_NAME?admin=hyperseek&action=affiliate_report&affiliate_id=$account[acct_id]\";'>
                           <td>
                             $account[acct_number]
                           </td>
                           <td>
                             $account[contact_name]
                           </td>
                           <td>
                             <a href='$SCRIPT_NAME?admin=accounts&action=$action&affiliate=$account[acct_id]'>$account[acct_id]</a>
                           </td>
                           <td>
                             $account[affiliate_type]
                           </td>
                           <td>
                             $account[payment_trigger]
                           </td>
                           <td>
                             $account[payment_type]
                           </td>
                           <td>$tax_form_link</td>
                         </tr>
                      ";
								}
						}
						$title = $language[ aff_list_title ];
						if ( $pending ) {
								$title = $language[ aff_list_title_pending ];
						}
						$description = $language[ aff_list_howto ];
						if ( $pending ) {
								$description = $language[ aff_list_pending_howto ];
								$search_form = '';
						}
						## Navigation ##
						if ( $total > 50 ) {
								$navlinks = array ();
								$numpages = floor ( $total / 50 );
								if ( $total % 50 ) {
										$numpages++;
								}
								for ( $x = 1; $x <= $numpages; $x++ ) {
										if ( $x == $page ) {
												array_push ( $navlinks , "<b>$x</b>" );
										} else {
												array_push ( $navlinks , "<a href='$SCRIPT_NAME?admin=accounts&action=list_affiliates&page=$x'>$x</a>" );
										}
								}
								$navbar = "... Page: " . implode ( " | " , $navlinks );
						} else {
								$navbar = '';
						}
						if ( $CGI[ 'nl' ] ) {
								$OUTPUT = "
                  <table border=0 width=400><tr><td>
                   $description
                  </td></tr></table>
                  <br />
                  $search_form
                ";
						} else {
								$OUTPUT = "
                  <table border=0 width=400><tr><td>
                   $description
                  </td></tr></table><br />

                   $search_form

                   <br />

                   $start to $end of $total
                   $navbar
                   <br /><br />

                   <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
                   <script language='javascript' src='$config[script_dir]/tablesort.js'></script>


                    <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                    <thead>
                        <tr>
                            <td>$language[aff_list_number]</td>
                            <td>$language[aff_list_name]</td>
                            <td>$language[aff_list_id]</td>
                            <td>$language[aff_list_type]</td>
                            <td>$language[aff_list_paid_by]</td>
                            <td>$language[aff_list_paid_via]</td>
                            $tax_header
                        </tr>
                    </thead>
                    <tbody>
                       $list_ie
                    </tbody>
                    </table>

                    <br />
                    $start to $end of $total
                    $navbar
                ";
						}
						admin_output ( $title , $OUTPUT );
				}

				function print_tax_form ()
				{
						global $disabled;
						global $submit;
						global $CGI;
						global $oSQL;
						global $manager;
						global $forms;
						global $date_and_time;
						global $SCRIPT_NAME;
						global $data_dir;
						include_once ( "$data_dir/custom/taxforms.inc" );
						$id      = $CGI[ 'affiliate' ];
						$SQL     = "SELECT * FROM iweb_affiliate_tax_forms WHERE affiliate = '$id'";
						$taxinfo = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
						$finfo   = $taxinfo[ 'formdata' ];
						$stmt    = "\$forminfo = $finfo;";
						eval ( $stmt );
						$formname      = $forminfo[ 'form' ];
						$disabled      = "disabled";
						$submit        = "hidden";
						$date_and_time = "";
						$style_view    = "
             <style>
               input,radio,checkbox,textarea,select { border:none; }
             </style>
           ";
						read_forms ( $forminfo );
						$date = date ( "m/d/Y h:i" , $taxinfo[ 'timestamp' ] );
						$ip   = $taxinfo[ 'ip_number' ];
						echo $style_view . $forms[ $formname ];
						echo "<b>Signed: </b> $date<br /><b>via IP</b> $ip";
						exit;
				}

				function affiliate_edit ( $error = '' )
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						global $tempaltes;
						global $data_dir;
						global $assigned_feeds;
						global $assigned_cats;
						$record = new Record( array ( "Parent" => $oSQL , "Table" => AFF_INFO_TABLE , "Schema" => AFF_INFO_TABLE , "Lookup" => 1 , "affiliate" => $CGI[ affiliate ] ) );
						$record->update_values ( $CGI );
						$assigned_feeds = $record->data[ xml_feeds ];
						$formarray      = $record->generate_form ();
						$form           = '';						
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								if ( preg_match ( "/selected_cats/i" , $fieldinput ) ) {
										$prompt .= "(<a href='javascript:selectcats(\"$CGI[affiliate]\")'>Choose</a>)";
								}
								$form .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$tax_form_link = '';
						if ( file_exists ( "$data_dir/custom/taxforms.inc" ) ) {
								$tax_form_link = " &#183; <a href='$SCRIPT_NAME?admin=accounts&action=print_tax_form&affiliate=$CGI[affiliate]'>View Tax Form</a>";
						}
						$OUTPUT = "
                <br />
                <blockquote>
                  <div style='width:400px;color:red'>$error</div>
                  <form name='aform' action='$SCRIPT_NAME' method='post'>
                  <table cellpadding=8 cellspacing=0 border=0>
                    <tr bgcolor='navy'>
                      <td colspan=2>
                        <font color='yellow'>
                         Affiliate Information
                        </font>
                      </td>
                    </tr>
                    <tr bgcolor='silver'>
                      <td colspan=2>
                          <b>Menu:</b> &nbsp;
                          <a href='$SCRIPT_NAME?admin=accounts&action=edit_account&acct_id=$CGI[affiliate]'>$language[aff_edit_acct_link]</a> &#183;
                          <a href='$SCRIPT_NAME?admin=hyperseek&action=affiliate_report&affiliate_id=$CGI[affiliate]'>$language[aff_edit_report_link]</a> &#183;
                          <a href='$SCRIPT_NAME?admin=accounts&action=adjust_balance&acct_id=$CGI[affiliate]&form=1'>$language[aff_edit_adj_link]</a> &#183;
                          <a href='$SCRIPT_NAME?admin=hyperseek&action=affiliate_payments&affiliate=$CGI[affiliate]'>$language[aff_edit_payment_link]</a> &#183;
                          <a href='$SCRIPT_NAME?admin=hyperseek&action=affiliate_payment_hist&affiliate=$CGI[affiliate]'>$language[aff_edit_history_link]</a>
                          $tax_form_link
                      </td>
                    </tr>

                    $form

                    <tr>
                      <td colspan=2>
                       <center>
                         <!--
                         <select name='action'>
                            <option value='affiliate_save'>$language[aff_edit_save]</option>
                            <option value='affiliate_delete'>$language[aff_edit_delete]</option>
                         </select>
                         -->
                         <input type='submit' value='$language[aff_edit_save]'>
                         <input type='hidden' name='admin' value='accounts'>
                         <input type='hidden' name='action' value='affiliate_save'>
                       </center>
                      </td>
                    </tr>
                 </table>
                 </form>
                 </blockquote>

                 <script language='javascript'>
                     function selectcats(aff) {
                         var URL = '$SCRIPT_NAME?admin=accounts&action=aff_categories&affiliate=' + aff;
                         cats=window.open (URL,'X','toolbar=no,scrollbars=yes,status=yes,directories=no,resizable=yes,menubar=no,width=625,height=500')
                     }
                 </script>
           ";
						admin_output ( "$language[aff_edit_title] <b>$CGI[affiliate]</b>" , $OUTPUT );
				}

				function affiliate_save ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						global $tempaltes;
						$message = '';
						$record  = new Record( array ( "Parent" => $iSQL , "Table" => AFF_INFO_TABLE , "Schema" => AFF_INFO_TABLE , "Lookup" => 1 , "affiliate" => $CGI[ 'affiliate' ] ) );
						if ( $CGI[ 'affiliate_type' ] == "PPC Manager" ) {
								$ok = 0;
								foreach ( $CGI[ 'payment_trigger' ] as $trigger ) {
										if ( $trigger == "revshare" ) {
												$ok = 1;
										}
								}
								if ( !$ok ) {
										$error = "
                     <b>PPC Manager</b> is a special affiliate type where the Affiliate has control over the
                     account's bidding, listings, etc.   They are a 'virtual employee' ... and as such, the
                     only 'payment trigger' that is valid for these types of affiliates is <b>revshare</b>.
                     <br /><br />
                     Please review this affiliate's information, make sure that you set them up to trigger payment
                     on revshare, and that you've indicated a revshare percentage.  Their earnings will be based
                     on this percentage of all of their customer's deposits.
                  ";
										$this->affiliate_edit ( $error );
								}
						}
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										$message = $language[ aff_save_ok ];
										if ( $CGI[ 'affiliate_type' ] == "PPC Manager" ) {
												$group_name = "PPC Manager";
										} else {
												$group_name = "Affiliate";
										}
										// Make sure that they are assigned to the right group //
										$grecord                       = new Record( array ( "Parent" => $iSQL , "Table" => GROUP_ASSIGNMENTS_TABLE , "Schema" => GROUP_ASSIGNMENTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ 'affiliate' ] , "group_name" => $group_name ) );
										$grecord->data[ 'acct_id' ]    = $CGI[ 'affiliate' ];
										$grecord->data[ 'group_name' ] = $group_name;
										$grecord->update_values ( $grecord->data );
										$grecord->save ();
								} else {
										$message = "$language[aff_save_error] <pre>$record->error_text</pre>";
								}
						} else {
								$message = "$language[aff_save_error] <pre>$record->error_text</pre>";
						}
						admin_output ( $language[ aff_save_title ] , $message );
				}

				function affiliate_delete ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						global $my_accounts;
						global $SCRIPT_NAME;
						### IF they want to delete (give options for what level to delete: just this, acct info too, or total nuke)
						### Make sure and delete it fully.
						admin_output ( "$language[aff_delete_title] $CGI[affiliate]" , $language[ aff_delete_warning ] );
				}
		} // End Class


		// -------------------------------------------------------------------------- //
		// Generic Functions (helpers) used to populate drop lists, etc for SQL
		// Forms. These need to be part of a class.
		// -------------------------------------------------------------------------- //

		function list_xml_feeds ( $params = '' )
		{
				global $oSQL;
				global $assigned_feeds;
				$feeds = split ( ',' , $assigned_feeds );
				foreach ( $feeds as $feed ) {
						$assigned[ $feed ] = 1;
				}
				$options = '';
				$SQL     = "SELECT engine_name FROM " . XML_FEEDS_TABLE . " ORDER BY engine_name";
				$list    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				foreach ( $list as $engine ) {
						$selected = '';
						if ( $assigned[ $engine ] ) {
								$selected = "selected='selected'";
						}
						$options .= " <option value='$engine' $selected>$engine</option> ";
				}

				return $options;
		}

		function aff_categories ()
		{
				global $oSQL;
				global $CGI;
				$acats = array ();
				if ( $CGI[ affiliate ] ) {
						$SQL           = "SELECT selected_cats FROM " . AFF_INFO_TABLE . " WHERE affiliate = '$CGI[affiliate]'";
						$assigned_cats = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						foreach ( split ( ';' , $assigned_cats ) as $id ) {
								if ( $id ) {
										$acats[ $id ] = 1;
								}
						}
				}
				$SQL     = "SELECT category_id, parent_id, full_name FROM " . CATEGORY_TABLE . " ORDER BY full_name ASC";
				$cats    = $oSQL->search ( array ( "SQL" => $SQL ) );
				$options = '';
				foreach ( $cats as $category ) {
						$cid     = $category[ category_id ];
						$checked = '';
						if ( $acats[ $cid ] ) {
								$checked = 'checked';
						}
						$spaces = '';
						$carr   = split ( ':' , $category[ full_name ] );
						foreach ( $carr as $xyz ) {
								$spaces .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
						}
						$options .= "
              $spaces <input type='checkbox' name='category' value='$cid' $checked />$category[full_name]<br />
            ";
				}
				echo "
           <html>

           <head>
              <title>Affiliate Category Select</title>

              <script language='javascript'>
                   function do_assignment() {
                      var cats = document.cform.category;
                      var selcats = '';
                      for (i = 0; i < cats.length; i++) {
	                     if ( cats[i].checked == true ) {
                            if ( selcats ) { selcats = selcats + ';'; }
                            selcats = selcats + cats[i].value;
                         }
                      }
                      var saveto = opener.document.aform.selected_cats;
                      saveto.value=selcats;
                      self.close();
                   }
              </script>

           </head>

           <form name='cform'>
               <h3>Choose Categories</h3>
               $options
               <br />
               <center>
                 <input type='button' onClick='do_assignment()' value='Assign Selected Categories' />
               </center>
           </form>
           </html>
        ";
				exit;
		}

?>
