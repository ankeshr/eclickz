<?php

		 /*

		    ________________________________________________________________________________

			     PROGRAM : ASP : Admin : Core Admin Center
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


		#-------------------------------------------------------------------------#
		# Admin Functions
		#-------------------------------------------------------------------------#

		function manage_bids ( $message = '' )
		{
				global $CGI;
				global $language;
				global $SCRIPT_NAME;
				global $config;
				global $oSQL;
				global $my_accounts;

				// echo "<pre>";
				// print_r($CGI);
				// echo "</pre>";
				// die;

				// Check that sales reps using this admin center are allowed access
				if ( count ( $my_accounts ) >= 1 && !$my_accounts[ $CGI[ acct_id ] ] ) {
						admin_output ( "$language[manage_bids_title] $CGI[acct_id]" , $language[ acct_access_denied ] );
				}
				$account = new Account( array ( "Parent" => $oSQL , "Table" => ACCOUNTS_TABLE , "Schema" => ACCOUNTS_TABLE , "Lookup" => 1 , "acct_id" => $CGI[ acct_id ] ) );
				$output  = '';
				$menu    = '';
				$balance = $account->data[ current_balance ];
				if ( !( $account->data[ acct_id ] && ( $CGI[ acct_id ] == $account->data[ acct_id ] ) ) ) {
						$output = "<span style='color:red'><b>$CGI[acct_id]:</b> $language[invalid_account]</span>";
						admin_output ( "$language[invalid_account] $CGI[acct_id]" , $output );
				}
				$links      = $account->get_links ();
				$valid_link = '';
				$navlinks   = '';
				foreach ( $links as $link ) {
						if ( $CGI[ id ] == $link[ id ] ) {
								$menu .= " <li><b><font color='blue'>$link[title]</font></b> ";
								$valid_link = $link[ id ];
						} else {
								$menu .= "<li style='padding:8px'><a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&acct_id=$CGI[acct_id]&id=$link[id]'>$link[title]</a><br />$link[url]";
						}
				}
				if ( $valid_link && $CGI[ 'do' ] == "list" ) {
						$link        = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $valid_link ) );
						$title       = $link->data[ title ];
						$desc        = $link->data[ description ];
						$url         = $link->data[ url ];
						$keywords    = $link->fetch_bids ();
						$categories  = $link->fetch_categories ();
						$keywordlist = '';
						$start       = $CGI[ start ];
						$end         = $start + 10;
						$total       = count ( $keywords );
						$pages       = $total / 10;
						for ( $x = 0; $x <= $pages; $x++ ) {
								$show   = $x + 1;
								$nstart = $x * 10;
								if ( $navlinks ) {
										$navlinks .= " | ";
								}
								$navlinks .= "<a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&do=list&acct_id=$CGI[acct_id]&id=$valid_link&start=$nstart'>$show</a>";
						}
						$cnt = 0;
						foreach ( $keywords as $keyword => $price ) {
								$cnt++;
								if ( $cnt > $end ) {
										break;
								}
								if ( $cnt <= $start ) {
										continue;
								}
								$show_keyword = $keyword;
								if ( $zone ) {
										$show_keyword = $word . " [$zone]";
								}
								$price = $keywords[ $keyword ];
								$eword = urlencode ( $keyword );
								$keywordlist .= "
                      <tr>
                       <td>
                         <font color='blue'>$show_keyword</font>
                       </td>
                       <td>
                         [<a href='$SCRIPT_NAME?admin=hyperseek&action=delete_bid&word=$eword&acct_id=$CGI[acct_id]&id=$valid_link'>$language[manage_bids_delete]</a>]
                       </td>
                       <td>
                         [<a href='$SCRIPT_NAME?admin=hyperseek&action=delete_bid&do=forbid_term&word=$eword&acct_id=$CGI[acct_id]&id=$valid_link'>$language[manage_bids_forbid]</a>]
                       </td>
                       <td>
                         <input name='update[$keyword]' value='$price' size=5>
                       </td>
                      </tr>
                ";
						}
						foreach ( $categories as $keyword ) {
								$cat     = new Record( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $keyword->data[ category ] ) );
								$catname = $cat->data[ full_name ];
								$catid   = $keyword->data[ category ];
								$price   = $keyword->data[ bid_price ];
								$keywordlist .= "
                      <tr>
                       <td colspan='3'>
                         $language[manage_bids_category] <font color='blue'>$catname</font>
                       </td>
                       <td>
                         <input name='updatecat[$catid]' value='$price' size=5>
                       </td>
                      </tr>
               ";
						}
						$output = "
               <b>$title</b><br />
               <a href='$url' target='new'>$url</a><br />
               <i>$desc</i><hr>
               <form action='$SCRIPT_NAME' method='post'>
               <table>
                 <tr>
                   <th>$language[manage_bids_keyword]</th>
                   <th colspan=2>$language[manage_bids_action]</th>
                   <th>$language[manage_bids_amount]</th>
                 </tr>
                 $keywordlist

                 <tr>
                   <td colspan=4 align='center'>
                     <center>$language[page_jump_prompt] $navlinks</center>
                   </td>
                 </tr>

                 <tr>
                   <td colspan=4 align='center'>
                      <input type='hidden' name='action' value='change_bids'>
                      <input type='hidden' name='admin' value='hyperseek'>
                      <input type='hidden' name='id' value='$valid_link'>
                      <input type='submit' value='$language[manage_bids_button]'>
                   </td>
                 </tr>
               </table>
               </form>
            ";
				} elseif ( $valid_link ) {
						$link   = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $valid_link ) );
						$title  = $link->data[ title ];
						$desc   = $link->data[ description ];
						$url    = $link->data[ url ];
						$output = "
              <b>$title</b><br />
              <a href='$url' target='new'>$url</a><br />
              <i>$desc</i>
              <br /><br />

              <a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&do=list&acct_id=$CGI[acct_id]&id=$valid_link'>Micro Manage</a>
              <br />
              <a href='$SCRIPT_NAME?admin=hyperseek&action=nuke_bids&link_id=$CGI[id]&acct_id=$CGI[acct_id]'>Nuke All Keywords</a>
              <br />
              <a href='$SCRIPT_NAME?admin=hyperseek&action=deactivate_bids&link_id=$CGI[id]&acct_id=$CGI[acct_id]'>De-activate Bids</a>
              <br />
              <a href='$SCRIPT_NAME?admin=hyperseek&action=reactivate_bids&link_id=$CGI[id]&acct_id=$CGI[acct_id]'>Re-activate Bids</a>
              <br />
           ";
				} else {
						if ( $CGI[ id ] ) {
								$output = $language[ manage_bids_no_terms ];
						}
				}
				$OUT = "
          <font color='red'>$message</font>
          <table border=0 cellpadding=10>
            <tr>
              <td valign='top' nowrap>
                <b>$language[listings_prompt]</b><br />
                <ul>$menu</ul>
              </td>
              <td valign='top'>
                $language[bidded_terms_prompt]
                $output
              </td>
            </tr>
          </table>
        ";
				admin_output ( "$language[manage_bids_title] $CGI[acct_id]" , $OUT );
		}

		function reactivate_bids ()
		{
				global $oSQL;
				global $language;
				global $SCRIPT_NAME;
				global $CGI;
				$link   = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ link_id ] ) );
				$title  = $link->data[ title ];
				$retval = "Reactivating bids for: $CGI[link_id] / $title\n";
				$retval .= $link->reactivate_bids ();
				$output = "
          <pre>$retval</pre>
          <a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&acct_id=$CGI[acct_id]&id=$CGI[link_id]'>Return</a>
        ";
				admin_output ( "$language[manage_bids_title] $title" , $output );
		}


		function deactivate_bids ()
		{
				global $oSQL;
				global $language;
				global $SCRIPT_NAME;
				global $CGI;
				$link   = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ link_id ] ) );
				$title  = $link->data[ title ];
				$retval = "Deactivating bids for: $CGI[link_id] / $title\n";
				$retval .= $link->negate_bids ();
				$output = "
          <pre>$retval</pre>
          <a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&acct_id=$CGI[acct_id]&id=$CGI[link_id]'>Return</a>
        ";
				admin_output ( "$language[manage_bids_title] $title" , $output );
		}

		function nuke_bids ()
		{
				global $oSQL;
				global $language;
				global $SCRIPT_NAME;
				global $CGI;
				$link   = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ link_id ] ) );
				$title  = $link->data[ title ];
				$retval = "Nuking bids for: $CGI[link_id] / $title\n";
				$retval .= $link->nuke_bids ();
				$output = "
          <pre>$retval</pre>
          <a href='$SCRIPT_NAME?admin=hyperseek&action=manage_bids&acct_id=$CGI[acct_id]&id=$CGI[link_id]'>Return</a>
        ";
				admin_output ( "$language[manage_bids_title] $title" , $output );
		}

		function change_bids ()
		{
				global $iSQL;
				global $language;
				global $config;
				global $SCRIPT_NAME;
				global $CGI;
				$id    = $CGI[ id ];
				$link  = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $id ) );
				$title = $link->data[ title ];
				$desc  = $link->data[ description ];
				foreach ( $CGI[ 'updatecat' ] as $key => $bid ) {
						$category = new Record( array ( "Parent" => $iSQL , "Table" => CATS_TABLE , "Schema" => CATS_TABLE , "Lookup" => 1 , "id" => $id , "category" => $key ) );
						if ( $category->data[ bid_price ] != $bid ) {
								$category->data[ bid_price ] = $bid;
								if ( $category->update_values ( $category->data ) ) {
										if ( $category->save () ) {
												$LOG .= "$language[updating] (Cat) $key --&gt; $language[dollar_sign] $bid<br />\n";
										}
								}
						} else {
								$LOG .= "Skipped Category $key (Same Bid)<br />\n";
						}
				}
				foreach ( $CGI[ 'update' ] as $key => $bid ) {
						list ( $word , $zone ) = split ( '~' , $key );
						$currentword = new Record( array ( "Parent" => $iSQL , "Table" => KEYS_TABLE , "Schema" => KEYS_TABLE , "Lookup" => 1 , "id" => $id , "keyword" => $word , "zone" => $zone ) );
						if ( $currentword->data[ bid_price ] != $bid ) {
								$currentword->data[ bid_price ] = $bid;
								if ( $currentword->update_values ( $currentword->data ) ) {
										if ( $currentword->save () ) {
												$LOG .= "$language[updating] (keyword) $word --&gt; $language[dollar_sign] $bid<br />\n";
										}
								}
						} else {
								$LOG .= "Skipped Keyword $word (Same Bid)<br />\n";
						}
				}
				$OUTPUT = "
          $language[manage_bids_update_header] <b>$title</b><br /><i>$desc</i><br /><br />
          $LOG
          <br /><br />
          <center><a href='javascript:history.go(-1)'>$language[return_link]</a></center>
       ";
				admin_output ( $language[ manage_bids_update_title ] , $OUTPUT );
		}

		function delete_bid ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				$term    = $CGI[ 'word' ];
				$id      = $CGI[ 'id' ];
				$message = '';
				if ( $term ) {
						$currentword = new Record( array ( "Parent" => $iSQL , "Table" => KEYS_TABLE , "Schema" => KEYS_TABLE , "Lookup" => 1 , "id" => $id , "keyword" => $term ) );
						if ( $currentword->data[ keyword ] != $term ) {
								$message = "$language[delete_bid_invalid] <b>$term</b>";
						} else {
								$currentword->delete ();
								$message = "<li>$term</li>";
						}
						if ( $CGI[ 'do' ] == "forbid_term" ) {
								$forbid               = new Record( array ( "Parent" => $iSQL , "Table" => FORBID_TABLE , "Schema" => FORBID_TABLE ) );
								$forbid->data[ id ]   = $id;
								$forbid->data[ word ] = $term;
								if ( $forbid->update_values ( $forbid->data ) ) {
										if ( $forbid->save () ) {
												$message .= "<br /><b>$term</b> $language[forbid_bid_ok]";
										} else {
												$message .= "<br /><b>$term</b> $language[forbid_bid_error] <pre>$forbid->error_string</pre>";
										}
								} else {
										$message .= "<br /><b>$term</b> $language[forbid_bid_error] <pre>$forbid->error_string</pre>";
								}
						}
				}
				manage_bids ( $message );
		}


		function pending_bids ( $accts = '' )
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				global $PPCMANAGER;
				$accounts_and = '';
				// Table Vars
				$pendingbids = PENDING_BIDS_TABLE;
				$linkstable  = LINKS_TABLE;
				if ( count ( $my_accounts ) >= 1 ) {
						$list         = join ( "','" , array_keys ( $my_accounts ) );
						$accounts_and = " AND $pendingbids.account IN('$list')";
				}
				if ( $CGI[ account ] ) {
						$accounts_and = " AND $pendingbids.account = '$CGI[account]'";
				}
				// Force a bad query if this is a ppc manager doing the lookup, and they don't have any accounts
				if ( $PPCMANAGER && !$accounts_and ) {
						$accounts_and = "AND 1=2";
				}
				$list    = '';
				$SQL     = "
          SELECT $pendingbids.*, $linkstable.* FROM $pendingbids, $linkstable
          WHERE $pendingbids.linkid = $linkstable.id
          $accounts_and
          AND $pendingbids.account = $linkstable.account
       ";
				$pending = $oSQL->search ( array ( "SQL" => $SQL ) );
				aasort ( $pending , "+account,+title" );
				foreach ( $pending as $site ) {
						$pwords = split ( ',' , $site[ words ] );
						$num    = count ( $pwords );
						$list .= "
             <tr>
               <td><b>$site[account]</b></td>
               <td><a href='$SCRIPT_NAME?admin=hyperseek&action=edit_pending_bids&id=$site[id]'>$site[title]</a></td>
               <td align='right'>$num</td>
             </tr>
          ";
				}
				$OUTPUT = "
           <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
           <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

            <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
            <thead>
                <tr>
                    <td style='width: 100px;'>$language[pending_bids_account]</td>
                    <td style='width: 160px;'>$language[pending_bids_site]</td>
                    <td style='width: 100px;'>$language[pending_bids_numwords]</td>
                </tr>
            </thead>
            <tbody>
               $list
            </tbody>
            </table>
       ";
				admin_output ( $language[ pending_bids_title ] , $OUTPUT );
		}

		function edit_pending_bids ()
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$output   = '';
				$table    = '';
				$link     = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
				$title    = $link->data[ title ];
				$desc     = $link->data[ description ];
				$SQL      = "SELECT words FROM " . PENDING_BIDS_TABLE . " WHERE linkid='$CGI[id]'";
				$keys     = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
				$start    = $CGI[ start ];
				$end      = $start + 10;
				$words    = split ( ',' , $keys );
				$total    = count ( $words ) + 1;
				$pages    = $total / 10;
				$navlinks = '';
				for ( $x = 0; $x <= $pages; $x++ ) {
						$show = $x + 1;
						$st   = $x * 10;
						if ( $navlinks ) {
								$navlinks .= " | ";
						}
						$navlinks .= " <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_pending_bids&id=$CGI[id]&start=$st'>$show</a> ";
				}
				$cnt = 0;
				foreach ( split ( ',' , $keys ) as $keyword ) {
						list ( $word , $bid ) = split ( ':' , $keyword );
						$encword = urlencode ( $word );
						$cnt++;
						if ( $cnt > $end ) {
								break;
						}
						if ( $cnt <= $start ) {
								continue;
						}
						list ( $actualword , $zone ) = split ( '~' , $word );
						if ( $zone ) {
								$word = $actualword . " [$zone]";
						}
						$table .= "
              <tr>
                 <td><b>$word</b></td>
                 <td><b>$bid</b></td>
                 <td align='center'><input type='checkbox' name='approve[]' value='$keyword'></td>
                 <td align='center'><input type='checkbox' name='delete[]' value='$keyword'></td>
              </tr>
           ";
				}
				$OUTPUT = "

          <script language='javascript'>
               function Approve(c_how) {
                  for (var i=0;i<document.submit_form.elements.length;i++) {
                     var e=document.submit_form.elements[i];
                     e.checked = false;
                  }

                  if(c_how == 0) {
                     for (var i=0;i<document.submit_form.elements.length;i++) {
                        var e=document.submit_form.elements[i];
                        if (e.name == 'delete') {  e.checked = true; }
                     }
                  }
                  if(c_how == 1) {
                     for (var i=0;i<document.submit_form.elements.length;i++) {
                        var e=document.submit_form.elements[i];
                        if (e.name == 'approve') { e.checked = true; }
                     }
                  }
               }
          </script>
          <form name='submit_form' method='post' action='$SCRIPT_NAME'>
          <b>$title</b><br />
          <i>$desc</i><br />


           <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
           <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

            <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
            <thead>
                <tr>
                    <td style='width: 60px;'>Term</td>
                    <td style='width: 60px;'>Bid</td>
                    <td style='width: 60px;'>$language[edit_pending_bids_add]</td>
                    <td style='width: 60px;'>$language[edit_pending_bids_delete]</td>
                </tr>
            </thead>
            <tbody>
               $table
            </tbody>

            <tr>
              <td colspan=4 align='center'>
                 <center>$language[page_jump_prompt] $navlinks</center>
              </td>
            </tr>

            </table>

            <input type='hidden' name='action' value='do_pending_bids'>
            <input type='hidden' name='admin' value='hyperseek'>
            <input type='hidden' name='id' value='$CGI[id]'>

            <br /><br />
            <select name='how'>
              <option value='shown'>$language[pending_as_shown]</option>
              <option value='approve_all'>$language[pending_approve_all]</option>
              <option value='deny_all'>$language[pending_deny_all]</option>
            </select>

            <input type='submit' value='$language[edit_pending_bids_button]'>

          </form>

       ";
				admin_output ( "$language[edit_pending_title] $title" , $OUTPUT );
		}

		function do_pending_bids ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$link     = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
				$title    = $link->data[ title ];
				$desc     = $link->data[ description ];
				$id       = $link->data[ id ];
				$templist = new Record( array ( "Parent" => $iSQL , "Table" => PENDING_BIDS_TABLE , "Schema" => PENDING_BIDS_TABLE , "Lookup" => 1 , "linkid" => $id ) );
				$words    = split ( ',' , $templist->data[ words ] );
				foreach ( $words as $word ) {
						$pending[ $word ] = 1;
				}
				// First, re-write the CGI[delete] and CGI[approve] as hash arrays, if sent.
				if ( $CGI[ 'delete' ] ) {
						$tmp = array ();
						foreach ( $CGI[ 'delete' ] as $del ) {
								$tmp[ $del ] = 1;
						}
						$CGI[ 'delete' ] = $tmp;
				}
				if ( $CGI[ 'approve' ] ) {
						$tmp = array ();
						foreach ( $CGI[ 'approve' ] as $del ) {
								$tmp[ $del ] = 1;
						}
						$CGI[ 'approve' ] = $tmp;
				}
				// Override CGI[delete] and CGI[approve] if a global option was picked;
				if ( $CGI[ 'how' ] == "deny_all" ) {
						$CGI[ 'delete' ] = $pending;
				} elseif ( $CGI[ 'how' ] == "approve_all" ) {
						$CGI[ 'approve' ] = $pending;
				}
				foreach ( $CGI[ 'delete' ] as $dkey => $ok ) {
						$pending[ $dkey ] = 0;
						$delete .= "<li>$dkey</li>";
				}
				foreach ( $CGI[ 'approve' ] as $akey => $ok ) {
						## Remove the temp entry
						$pending[ $akey ] = 0;
						list ( $pendingword , $bid ) = split ( ':' , $akey );
						list ( $word , $zone ) = split ( '~' , $pendingword );
						$add{$word}++;
						if ( $bid <= 0.00 ) {
								if ( preg_match ( "/monthly/i" , $config[ jackhammer_charge_for ] ) ) {
										$bid = "0.00";
								} elseif ( $config[ jackhammer_minimum_bid ] > 0.00 && $bid <= 0.00 ) {
										$bid = $config[ jackhammer_minimum_bid ];
								} else {
										$bid = "0.00";
								}
						}
						$today                                = date ( "Ymd" );
						$mphone                               = metaphone ( $word );
						$SQL                                  = "SELECT synonym FROM " . LEXICAL_TABLE . " WHERE word = '$word'";
						$lkey                                 = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$newkeyword                           = new Record( array ( "Parent" => $iSQL , "Table" => KEYS_TABLE , "Schema" => KEYS_TABLE , "Lookup" => 1 , "id" => $id , "keyword" => $word , "zone" => $zone ) );
						$newkeyword->data[ id ]               = $id;
						$newkeyword->data[ keyword ]          = $word;
						$newkeyword->data[ metaphone ]        = $mphone;
						$newkeyword->data[ lexical_key ]      = $lkey;
						$newkeyword->data[ bid_price ]        = $bid;
						$newkeyword->data[ proxy_max ]        = "0.00";
						$newkeyword->data[ last_modified ]    = $today;
						$newkeyword->data[ last_modify_type ] = "I";
						$newkeyword->data[ create_date ]      = $today;
						$newkeyword->data[ zone ]             = $zone;
						if ( $newkeyword->update_values ( $newkeyword->data ) ) {
								if ( $newkeyword->save () ) {
										$approve .= "<li>$word";
								} else {
										$approve .= "<li>$word: $language[proc_pending_bids_error] <pre>$newkeyword->error_string</pre>";
								}
						} else {
								$approve .= "<li>$word: $language[proc_pending_bids_error] <pre>$newkeyword->error_string</pre>";
						}
				}
				## Save or delete the pending record ...
				if ( $pending ) {
						$saveus = array ();
						foreach ( $pending as $pword => $save ) {
								if ( $save ) {
										array_push ( $saveus , $pword );
								}
						}
				}
				if ( count ( $saveus ) ) {
						$idlist                  = join ( ',' , $saveus );
						$templist->data[ words ] = $idlist;
						$templist->update_values ( $templist->data );
						$templist->save ();
						$output = "Saving remaining pending bids<br />";
				} else {
						$templist->delete ();
						$output = "Removing pending bids flag<br />";
				}
				$OUTPUT = "
          <b>$language[proc_pending_bids_deleting]</b><br />
          $delete
          <br /><br />
          <b>$language[proc_pending_bids_approving]</b><br />
          $approve
          <br /><br />
          $output
          <br /><br />
        ";
				admin_output ( "$language[proc_pending_bids_title] $title" , $OUTPUT );
		}


		function make_fraud ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;


				// echo "<pre>";
				// print_r($CGI);
				// echo "</pre>";
				// die;

				// Table Vars
				$source_summary = SOURCE_SUMMARY_TABLE;
				$aff_summary    = AFF_SUMMARY_TABLE;
				$daily_summary  = DAILY_SUMMARY_TABLE;
				$click_summary  = CLICK_SUMMARY_TABLE;
				$click_log      = CLICK_LOG_TABLE;
				$fraud_table    = CLICK_FRAUD_TABLE;
				$click_table    = CLICK_LOG_TABLE;
				if ( $config[ jackhammer_merge_tables ] == "YES" ) {
						$fraud_table = $fraud_table . "_" . $CGI[ tbl ];
						$click_table = $click_table . "_" . $CGI[ tbl ];
				}
				$SQL   = "SELECT * FROM $click_table WHERE hit_id = '$CGI[hit_id]'";
				$click = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
				## Insert a record into jh_click_fraud
				$fraud                       = new Record( array ( "Parent" => $iSQL , "Table" => $fraud_table , "Schema" => CLICK_FRAUD_TABLE ) );
				$fraud->data[ hit_id ]       = 0;
				$fraud->data[ click_tstamp ] = $click[ hit_time ];
				$fraud->data[ fraud_id ]     = $CGI[ fraud_code ];
				$fraud->data[ link_id ]      = $click[ link_id ];
				$fraud->data[ word ]         = $click[ word ];
				$fraud->data[ price ]        = $click[ price ];
				$fraud->data[ hit_date ]     = $click[ hit_date ];
				$fraud->data[ affiliate ]    = $click[ affiliate ];
				$fraud->data[ account ]      = $click[ account ];
				$fraud->data[ ppc_engine ]   = $click[ ppc_engine ];
				$fraud->data[ ip_number ]    = $click[ ip ];
				$fraud->data[ user_agent ]   = $click[ user_agent ];
				$fraud->data[ referer ]      = $click[ referer ];
				$fraud->data[ fraud_reason ] = $CGI[ notes ];
				if ( $fraud->update_values ( $fraud->data ) ) {
						if ( $fraud->save () ) {
								$ok = 1;
						} else {
								$error = $fraud->show_debug_info ();
								admin_output ( "ERROR" , $error );
						}
				} else {
						$error = $fraud->show_debug_info ();
						admin_output ( "ERROR" , $error );
				}
				## Delete the record from jh_click_log
				$delClickSQL = "DELETE FROM $click_table WHERE hit_id = '$CGI[hit_id]'";
				$iSQL->run ( $delClickSQL );
				## Compute Commission
				$net_price = 0; ## Don't subtract anything unless we have an affiliate ##
				if ( $click[ affiliate ] && $click[ affiliate ] != "HOUSE" ) {
						$aSQL            = "SELECT click_amount FROM " . AFF_INFO_TABLE . " WHERE affiliate = '$click[affiliate]'";
						$aff_commission  = $iSQL->search ( array ( "SQL" => $aSQL , "Return" => "single" ) );
						$aff_calc_factor = $aff_commission;
						if ( preg ( '/%/' , $aff_commission ) ) {
								$aff_calc_factor = preg_replace ( "/\%/" , "" , $aff_calc_factor );
								$aff_calc_factor = $aff_calc_factor * .01;
						}
						$net_price = $click[ price ] * $aff_calc_factor;
				}
				## Change counters in affiliate summary table
				if ( $click[ affiliate ] ) {
						$affSQL = "
               UPDATE $aff_summary SET
                 click_count = click_count - 1,
                 fraud_count = fraud_count + 1,
                 click_amount_gross = click_amount_gross - $click[price],
                 click_amount_net = click_amount_net - $net_price
               WHERE acct_id = '$click[affiliate]' AND summary_date = '$click[hit_date]'
           ";
						$iSQL->run ( $affSQL );
				}
				## Change counters in source summary table
				if ( $click[ ppc_engine ] ) {
						$srcSQL = "
               UPDATE $source_summary SET
                 click_count = click_count - 1,
                 fraud_count = fraud_count + 1,
                 click_amount_gross = click_amount_gross - $click[price],
                 click_amount_net = click_amount_net - $net_price
               WHERE source = '$click[ppc_engine]' AND summary_date = '$click[hit_date]'
           ";
						$iSQL->run ( $srcSQL );
				}
				## Change counters in account summary table
				if ( $click[ account ] ) {
						$acctSQL = "
               UPDATE $click_summary SET
                 click_count = click_count - 1,
                 fraud_count = fraud_count + 1,
                 click_amount_gross = click_amount_gross - $click[price],
                 click_amount_net = click_amount_net - $net_price
               WHERE acct_id = '$click[account]' AND click_date = '$click[hit_date]'
           ";
						$iSQL->run ( $acctSQL );
				}
				## Change counters in daily summary table
				$summarySQL = "
               UPDATE $daily_summary SET
                 click_count = click_count - 1,
                 fraud_count = fraud_count + 1,
                 click_amount_gross = click_amount_gross - $click[price],
                 click_amount_net = click_amount_net - $net_price
               WHERE summary_date = '$click[hit_date]'
           ";
				$iSQL->run ( $summarySQL );
				## Add funds back to the account, if there is one
				if ( $click{account} ) {
						$acctSQL = "UPDATE " . ACCOUNTS_TABLE . " SET current_balance = current_balance + $click[price] WHERE acct_id = '$click[account]'";
						$iSQL->run ( $acctSQL );
				}
				$OUTPUT = "
          <b>Made Fraud ... </b><br /><br />
          <b>Click ID: </b> $CGI[hit_id]<br />
          <b>Code: </b> $CGI[fraud_code]<br />
          <b>Reason: </b> $CGI[notes]<br />
        ";
				admin_output ( "Convert Click to Fraud" , $OUTPUT );
		}


		function make_click ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;

				// echo "<pre>";
				// print_r($CGI);
				// echo "</pre>";
				// die;
				
				// Table Vars
				$source_summary = SOURCE_SUMMARY_TABLE;
				$aff_summary    = AFF_SUMMARY_TABLE;
				$daily_summary  = DAILY_SUMMARY_TABLE;
				$click_summary  = CLICK_SUMMARY_TABLE;
				$click_log      = CLICK_LOG_TABLE;
				$fraud_table    = CLICK_FRAUD_TABLE;
				$click_table    = CLICK_LOG_TABLE;
				if ( $config[ jackhammer_merge_tables ] == "YES" ) {
						$fraud_table = $fraud_table . "_" . $CGI[ tbl ];
						$click_table = $click_table . "_" . $CGI[ tbl ];
				}
				$SQL   = "SELECT * FROM $fraud_table WHERE hit_id = '$CGI[hit_id]'";
				$click = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
				## Compute Commission
				$net_price      = 0; ## Don't subtract anything unless we have an affiliate ##
				$aff_commission = '';
				if ( $click[ affiliate ] && $click[ affiliate ] != "HOUSE" ) {
						$aSQL            = "SELECT click_amount FROM " . AFF_INFO_TABLE . " WHERE affiliate = '$click[affiliate]'";
						$aff_commission  = $iSQL->search ( array ( "SQL" => $aSQL , "Return" => "single" ) );
						$aff_calc_factor = $aff_commission;
						if ( preg_match ( '/%/' , $aff_commission ) ) {
								$aff_calc_factor = preg_replace ( "/\%/" , "" , $aff_calc_factor );
								$aff_calc_factor = $aff_calc_factor * .01;
						}
						$net_price = $click[ price ] * $aff_calc_factor;
				}
				## Insert a record into jh_click_fraud
				$hit                      = new Record( array ( "Parent" => $iSQL , "Table" => $click_table , "Schema" => CLICK_LOG_TABLE ) );
				$hit->data[ hit_id ]      = 0;
				$hit->data[ hit_time ]    = $click[ click_tstamp ];
				$hit->data[ link_id ]     = $click[ link_id ];
				$hit->data[ word ]        = $click[ word ];
				$hit->data[ gross_price ] = $click[ price ];
				$hit->data[ price ]       = $click[ price ];
				$hit->data[ hit_date ]    = $click[ hit_date ];
				$hit->data[ affiliate ]   = $click[ affiliate ];
				$hit->data[ account ]     = $click[ account ];
				$hit->data[ ppc_engine ]  = $click[ ppc_engine ];
				$hit->data[ ip ]          = $click[ ip_number ];
				$hit->data[ user_agent ]  = $click[ user_agent ];
				$hit->data[ referer ]     = $click[ referer ];
				$hit->data[ commission ]  = $aff_commission;
				if ( $hit->update_values ( $hit->data ) ) {
						if ( $hit->save () ) {
								$ok = 1;
						} else {
								$error = $hit->show_debug_info ();
								admin_output ( "ERROR" , $error );
						}
				} else {
						$error = $hit->show_debug_info ();
						admin_output ( "ERROR" , $error );
				}
				## Delete the record from jh_click_fraud
				$delFraudSQL = "DELETE FROM $fraud_table WHERE hit_id = '$CGI[hit_id]'";
				$iSQL->run ( $delFraudSQL );
				## Change counters in affiliate summary table
				if ( $click[ affiliate ] ) {
						$affSQL = "
               UPDATE $aff_summary SET
                 click_count = click_count + 1,
                 fraud_count = fraud_count - 1,
                 click_amount_gross = click_amount_gross + $click[price],
                 click_amount_net = click_amount_net + $net_price
               WHERE acct_id = '$click[affiliate]' AND summary_date = '$click[hit_date]'
           ";
						$iSQL->run ( $affSQL );
				}
				## Change counters in source summary table
				if ( $click[ ppc_engine ] ) {
						$srcSQL = "
               UPDATE $source_summary SET
                 click_count = click_count + 1,
                 fraud_count = fraud_count - 1,
                 click_amount_gross = click_amount_gross + $click[price],
                 click_amount_net = click_amount_net + $net_price
               WHERE source = '$click[ppc_engine]' AND summary_date = '$click[hit_date]'
           ";
						$iSQL->run ( $srcSQL );
				}
				## Change counters in account summary table
				if ( $click[ account ] ) {
						$acctSQL = "
               UPDATE $click_summary SET
                 click_count = click_count + 1,
                 fraud_count = fraud_count - 1,
                 click_amount_gross = click_amount_gross + $click[price],
                 click_amount_net = click_amount_net + $net_price
               WHERE acct_id = '$click[account]' AND click_date = '$click[hit_date]'
           ";
						$iSQL->run ( $acctSQL );
				}
				## Change counters in daily summary table
				$summarySQL = "
               UPDATE $daily_summary SET
                 click_count = click_count + 1,
                 fraud_count = fraud_count - 1,
                 click_amount_gross = click_amount_gross + $click[price],
                 click_amount_net = click_amount_net + $net_price
               WHERE summary_date = '$click[hit_date]'
           ";
				$iSQL->run ( $summarySQL );
				## Draw Funds from account, if there is one
				if ( $click[ account ] ) {
						$acctSQL = "UPDATE " . ACCOUNTS_TABLE . " SET current_balance = current_balance - $click[price] WHERE acct_id = '$click[account]'";
						$iSQL->run ( $acctSQL );
				}
				$OUTPUT = "
          <b>Made Click ... </b><br /><br />
          <b>Click ID: </b> $CGI[hit_id]<br />
          <b>Added to Balance: </b> $click[price]<br />
        ";
				admin_output ( "Convert Fraud to Valid Click" , $OUTPUT );
		}


		function pick_jackhammer_account_form ()
		{
				global $oSQL;
				$accounts = '';
				if ( $CGI[ type ] == "affiliate_id" ) {
						$SQL     = "SELECT * FROM " . AFF_INFO_TABLE;
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						aasort ( $results , "+affiliate" );
						foreach ( $results as $account ) {
								$accounts .= " <option value='$account[affiliate]'>$account[affiliate]</option>";
						}
				} elseif ( $CGI[ type ] == "source" ) {
						$SQL     = "SELECT * FROM " . XML_FEEDS_TABLE;
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						aasort ( $results , "+engine_name" );
						foreach ( $results as $account ) {
								$accounts .= "<option value='$account[engine_name]'>$account[engine_name]</option>";
						}
				} else {
						$SQL     = "SELECT * FROM " . ACCOUNTS_TABLE;
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						aasort ( $results , "+acct_id" );
						foreach ( $results as $account ) {
								$accounts .= " <option value='$account[acct_id]'>$account[acct_id] ($account[contact_name])</option>";
						}
				}
				echo "
          <html>
          <head>
             <title>Account List</title>
             <script language='javascript'>
                function UpdateParent(form) {
                   var idx = form.account.selectedIndex;
                   var acct = form.account.options[idx].value;
                   opener.document.report_form.$CGI[type].value = acct;
                   self.close();
                }
             </script>
          </head>

          <center>
            <form>
              $language[pick_account_select]
                <select name='account'>
                  <option value=''>$language[pick_account_all]</option>
                  $accounts
                </select>
                <br />
                <input type='button' value='$language[pick_account_button]' onclick='updateparent(this.form)'>
            </form>
          </center>
       ";
				exit;
		}


		function keyword_minimums ( $message = '' )
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$recalculate = '';
				if ( $config[ 'jackhammer_charge_for' ] == "MONTHLY KEYWORDS" ) {
						$recalculate = "
                <p align='right'>
                    <a href='$SCRIPT_NAME?admin=hyperseek&action=calculate_keyword_minimums'>Re-Calculate KW Minimums based on search count</a>
                </p>
            ";
				}
				$output = "
            <div style='border:1px solid black; padding:10px'>
            <form action='$SCRIPT_NAME'>
               $language[keyword_minimum_prompt]
               <input name='keywords' />
               <input type='submit' value='Search' />
               <input type='hidden' name='action' value='keyword_minimums' />
               <input type='hidden' name='admin' value='hyperseek' />
               <input type='hidden' name='minimumsearch' value='1' />
            </form>
            <form action='$SCRIPT_NAME'>
                  $language[keyword_minimum_keyword] <input name='newkeyword'>
                  $language[keyword_minimum_bid] <input name='newbid'>
                  <input type='submit' value='Add New Keyword' />
                  <input type='hidden' name='action' value='save_keyword_minimums' />
                  <input type='hidden' name='admin' value='hyperseek' />
            </form>
            $recalculate
            </div>
        ";
				if ( $CGI[ minimumsearch ] ) {
						$where = '';
						if ( $CGI[ keywords ] ) {
								$where = "WHERE keyword like '%$CGI[keywords]%'";
						}
						$SQL      = "SELECT * FROM " . MINIMUMS_TABLE . " $where ORDER BY keyword ASC";
						$keywords = $oSQL->search ( array ( "SQL" => $SQL ) );
						$output .= "
                <form action='$SCRIPT_NAME' method='post'>
                <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
                <table cellspacing='0' class='sorted'>
                  <thead>
                  <tr>
                    <td>$language[keyword_minimum_keyword]</td>
                    <td>$language[keyword_minimum_bid]</td>
                    <td>$language[keyword_minimum_delete]</td>
                  </tr>
                  </thead>

                  <tbody>
            ";
						foreach ( $keywords as $keyword ) {
								$output .= "
                    <tr>
                      <td><b>$keyword[keyword]</b></td>
                      <td>
                        <input size='10' name='keywordbid-$keyword[keyword]' value='$keyword[minimum_bid]' />
                      </td>
                      <td>
                        <input type='checkbox' name='keyworddelete-$keyword[keyword]' value='1' />
                      </td>
                    </tr>
                ";
						}
						$output .= "
                    <tr>
                       <td colspan='3' align='center'>
                           <input name='action' type='hidden' value='save_keyword_minimums' />
                           <input type='hidden' name='admin' value='hyperseek' />
                           <input type='submit' value='$language[keyword_minimum_save]' />
                       </td>
                    </tr>
                </tbody>
                </table>
                </form>
            ";
				}
				$OUTPUT = $output . $message;
				admin_output ( $language[ keyword_minimum_menu ] , $OUTPUT );
		}


		function save_keyword_minimums ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$output = "<b>Saving Minimums</b><br />";
				## Add new one, if specified ##
				if ( $CGI[ newkeyword ] && $CGI[ newbid ] ) {
						$record                      = new Record( array ( "Parent" => $iSQL , "Table" => MINIMUMS_TABLE , "Schema" => MINIMUMS_TABLE , "Lookup" => 1 , "keyword" => $CGI[ newkeyword ] ) );
						$record->data[ keyword ]     = $CGI[ newkeyword ];
						$record->data[ minimum_bid ] = $CGI[ newbid ];
						$record->update_values ( $record->data );
						$record->save ();
						$output .= "<b>$CGI[newkeyword]</b> -- $CGI[newbid]<br />";
				}
				## Go through the list
				foreach ( $CGI as $key => $value ) {
						if ( preg_match ( '/keywordbid-/i' , $key ) ) {
								$keyword                     = preg_replace ( "/keywordbid\-/" , "" , $key );
								$bid                         = $CGI[ $key ];
								$record                      = new Record( array ( "Parent" => $iSQL , "Table" => MINIMUMS_TABLE , "Schema" => MINIMUMS_TABLE , "Lookup" => 1 , "keyword" => $keyword ) );
								$currentbid                  = $record->data[ minimum_bid ];
								$record->data[ keyword ]     = $keyword;
								$record->data[ minimum_bid ] = $bid;
								if ( $bid != $currentbid ) {
										$record->update_values ( $record->data );
										$record->save ();
										$output .= "<b>$keyword</b> -- $bid<br />";
								}
						}
						if ( preg_match ( '/keyworddelete-/i' , $key ) ) {
								$keyword = preg_replace ( "/keyworddelete\-/" , "" , $key );
								$record  = new Record( array ( "Parent" => $iSQL , "Table" => MINIMUMS_TABLE , "Schema" => MINIMUMS_TABLE , "Lookup" => 1 , "keyword" => $keyword ) );
								$record->delete ();
								$output .= "<span style='color:red'><b>$keyword</b> Deleted</span><br />";
						}
				}
				keyword_minimums ( $output );
		}

		function calculate_keyword_minimums ( $cron = 0 )
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$status       = array ();
				$num_searches = $config[ 'jackhammer_keyword_bump_limit' ];
				$bump_amount  = $config[ 'jackhammer_keyword_bump' ];
				$min          = $config[ 'jackhammer_minimum_bid' ];
				$SQL          = "SELECT criteria, num_searches FROM " . LOG_SUMMARY_TABLE . " WHERE num_searches >= $num_searches AND search_type = 'TEXTSEARCH'";
				$keys         = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "fullhash" ) );
				foreach ( $keys as $keyword => $count ) {
						$increments                     = ceil ( $count / $num_searches );
						$bump                           = $increments * $bump_amount;
						$new_bid                        = $min + $bump;
						$record                         = new Record( array ( "Parent" => $iSQL , "Table" => MINIMUMS_TABLE , "Schema" => MINIMUMS_TABLE , "Lookup" => 1 , "keyword" => $keyword ) );
						$record->data[ 'keyword' ]      = $keyword;
						$record->data[ 'num_searches' ] = $count;
						$record->data[ 'minimum_bid' ]  = $new_bid;
						$record->update_values ( $record->data );
						$record->save ();
						array_push ( $status , "Changing $keyword to $new_bid ($count Searches = $increments * $bump_amount)" );
				}
				if ( $cron ) {
						return join ( "\n" , $status );
				} else {
						$output = join ( "<br />" , $status );
						keyword_minimums ( $output );
				}
		}

		function sponsor_boxes ( $message = '' )
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				// Table Vars
				$links_table  = LINKS_TABLE;
				$sponsorboxes = SPONSOR_BOX_TABLE;
				$output       = "
            <div style='border:1px solid black; padding:10px'>
            <form action='$SCRIPT_NAME'>
               $language[sponsor_boxes_prompt]
               <input name='keywords' />
               <input type='submit' value='Search' />
               <input type='hidden' name='action' value='sponsor_boxes' />
               <input type='hidden' name='admin' value='hyperseek' />
               <input type='hidden' name='sponsorboxes' value='1' />
            </form>
            </div>
        ";
				if ( $CGI[ sponsorboxes ] ) {
						$where = "AND $sponsorboxes.keywords like '%$CGI[keywords]%'";
						if ( !$CGI[ keywords ] ) {
								$where = '';
						}
						$SQL      = "SELECT $sponsorboxes.*, $links_table.title, $links_table.url, $links_table.description FROM $sponsorboxes, $links_table WHERE $links_table.id = $sponsorboxes.linkid $where ORDER BY $sponsorboxes.keywords ASC";
						$keywords = $oSQL->search ( array ( "SQL" => $SQL ) );
						$output .= "
                    <form action='$SCRIPT_NAME' method='post'>
                    <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
                    <table cellspacing='1' class='sorted'>
                      <thead>
                      <tr>
                        <td>$language[sponsor_boxes_keyword]</td>
                        <td>$language[sponsor_boxes_title]</td>
                        <td>$language[sponsor_boxes_expire]</td>
                        <td>$language[sponsor_boxes_delete]</td>
                      </tr>
                      </thead>
                      <tbody>
                ";
						foreach ( $keywords as $keyword ) {
								$expire_date = formatdate ( $keyword[ expire_date ] );
								$output .= "
                        <tr>
                          <td><b>$keyword[keywords]</b></td>
                          <td>
                             <a href='$keyword[url]' target='new'>$keyword[title]</a><br />
                             <i>$keyword[description]</i>
                          </td>
                          <td>$expire_date</td>
                          <td>
                            <input type='checkbox' name='keyworddelete-$keyword[linkid]' value='$keyword[keywords]' />
                          </td>
                        </tr>
                    ";
						}
						$output .= "
                        <tr>
                           <td colspan='4' align='center'>
                               <br /><br />
                               <input name='action' type='hidden' value='save_sponsor_boxes' />
                               <input type='hidden' name='admin' value='hyperseek' />
                               <input type='submit' value='$language[sponsor_boxes_save]' />
                           </td>
                        </tr>
                    </tbody>
                    </table>
                    </form>
                ";
				}
				$OUT = $message . $output;
				admin_output ( $language[ sponsor_boxes ] , $OUT );
		}


		function save_sponsor_boxes ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$output = "<b>Saving Sponsor Boxes</b><br />";
				## Go through the list
				foreach ( $CGI as $key => $keyword ) {
						if ( preg_match ( '/keyworddelete-/i' , $key ) ) {
								$id      = preg_replace ( "/keyworddelete\-/i" , "" , $key );
								$keyword = $CGI[ $key ];
								$record  = new Record( array ( "Parent" => $iSQL , "Table" => SPONSOR_BOX_TABLE , "Schema" => SPONSOR_BOX_TABLE , "Lookup" => 1 , "linkid" => $id , "keywords" => $keyword ) );
								$record->delete ();
								$output .= "<span style='color:red'><b>$keyword</b> Deleted for <b>$id</b></span><br />";
						}
				}
				sponsor_boxes ( $output );
		}


		#--------------------------------------------------------------------------#
		#  Promotional Codes                                                       #
		#--------------------------------------------------------------------------#
		function promotions ( $message = '' )
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$report     = '';
				$SQL        = "SELECT * FROM " . PROMOTIONS_TABLE;
				$promotions = $oSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $promotions as $promotion ) {
						$encp  = urlencode ( $promotion[ promo_code ] );
						$sd    = formatdate ( $promotion[ start_date ] );
						$ed    = formatdate ( $promotion[ expire_date ] );
						$valid = "$sd - $ed";
						$report .= "
                <tr>
                    <td><a href='$SCRIPT_NAME?admin=hyperseek&action=edit_promotion&promo_code=$encp'>$promotion[promo_code]</a></td>
                    <td>$promotion[promo_type]</td>
                    <td>$promotion[max_uses]</td>
                    <td>$promotion[num_uses]</td>
                    <td>$valid</td>
                    <td>$promotion[value]</td>
                </tr>
           ";
				}
				$np     = new Record( array ( "Parent" => $oSQL , "Table" => PROMOTIONS_TABLE , "Schema" => PROMOTIONS_TABLE ) );
				$fields = $np->generate_form ();
				$form   = '';
				while ( list ( $prompt , $fieldinput ) = each ( $fields ) ) {
						$form .= "<tr><td bgcolor='#e0e0e0'><b>$prompt</b></td><td bgcolor='#c0c0c0'>$fieldinput</td></tr>\n";
				}
				$OUTPUT = "
           <b>Current Promotions</b>

           <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
           <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

            <table cellspacing='0' class='sorted'>
            <thead>
                <tr>
                    <td>Code</td>
                    <td>Type</td>
                    <td>Quantity</td>
                    <td># Uses</td>
                    <td>Valid</td>
                    <td>Value</td>
                </tr>
            </thead>
            <tbody>
               $report
            </tbody>
            </table>

           <br />

           <hr />


           <form action='$SCRIPT_NAME' method='post'>
             <table cellspacing='0' cellpadding='3'>
                <tr>
                   <td colspan='2'><b>New Promotion:</b></td>
                </tr>
                $form
                <tr>
                   <td colspan='2' align='center'>
                      <input type='hidden' name='action' value='save_promotion' />
                      <input type='hidden' name='admin' value='hyperseek' />
                      <input type='submit' value='Save' />
                   </td>
                </tr>
             </table>
           </form>
       ";
				admin_output ( "Promotions  $message" , $OUTPUT );
		}

		function edit_promotion ( $message = '' )
		{
				global $oSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$np     = new Record( array ( "Parent" => $oSQL , "Table" => PROMOTIONS_TABLE , "Schema" => PROMOTIONS_TABLE , "Lookup" => 1 , "promo_code" => $CGI[ promo_code ] ) );
				$fields = $np->generate_form ();
				$form   = '';
				while ( list ( $prompt , $fieldinput ) = each ( $fields ) ) {
						$form .= "<tr><td bgcolor='#e0e0e0'><b>$prompt</b></td><td bgcolor='#c0c0c0'>$fieldinput</td></tr>\n";
				}
				$name   = $np->data[ promo_code ];
				$OUTPUT = "
           <form action='$SCRIPT_NAME' method='post'>
             <table cellspacing='0' cellpadding='3'>
                <tr>
                   <td colspan='2'><b>Edit Promotion:</b> $name</td>
                </tr>
                $form
                <tr>
                   <td colspan='2' align='center'>
                      <input type='hidden' name='action' value='save_promotion' />
                      <input type='hidden' name='admin' value='hyperseek' />
                      <input type='submit' value='Save' />
                   </td>
                </tr>
             </table>
           </form>
       ";
				admin_output ( "Edit Promotion ($name)" , $OUTPUT );
		}

		function save_promotion ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$np = new Record( array ( "Parent" => $iSQL , "Table" => PROMOTIONS_TABLE , "Schema" => PROMOTIONS_TABLE , "Lookup" => 1 , "promo_code" => $CGI[ promo_code ] ) );
				if ( $np->update_values ( $CGI ) ) {
						if ( $np->save () ) {
								promotions ( " <span style='color:red'>(Saved Promotion $CGI[promo_code])</span>" );
						} else {
								edit_promotion ( $np->error_string );
						}
				} else {
						edit_promotion ( $np->error_string );
				}
		}


		#################################################
		# mySQL Merge Table Management
		#################################################

		function merge_tables ()
		{
				global $iSQL;
				global $CGI;
				global $language;
				global $config;
				global $my_accounts;
				global $SCRIPT_NAME;
				$output = '';
				$header = '';
				if ( $config[ 'jackhammer_merge_tables' ] != "YES" ) {
						$message = "
               <a href='javascript:top.openHelp(\"hyperseek\",\"merge_tables\");'>Merge Tables</a> Not Activated.
               <br /><br />
               Read the documentation about Merge Tables (linked above).  If your system supports them, and if you
               elect to use them, <a href='$SCRIPT_NAME?admin=hyperseek&action=setup_merge_tables'>click this link</a> to turn them
               on, and begin using them.
           ";
						admin_output ( "Merge Table Management" , $message );
				}
				// Table Vars
				$click_log    = CLICK_LOG_TABLE;
				$click_fraud  = CLICK_FRAUD_TABLE;
				$click_debug  = CLICK_DEBUG_TABLE;
				$log_table    = LOG_TABLE;
				$source_table = 'iweb_source_log';
				if ( $CGI[ save_merge_assignments ] ) {
						$header = "<span style='color:blue'><b>Jackhammer Merge Table Definitions Saved</b></span><br /><br />";
						$cl     = implode ( "," , $CGI[ jh_click_log_assignments ] );
						$cf     = implode ( "," , $CGI[ jh_click_fraud_assignments ] );
						$dbg    = implode ( "," , $CGI[ jh_debug_assignments ] );
						$logs   = implode ( "," , $CGI[ hyperseek_log_assignments ] );
						$slogs  = implode ( "," , $CGI[ source_log_assignments ] );
						$SQL    = "DROP TABLE IF EXISTS " . CLICK_LOG_TABLE;
						$iSQL->run ( $SQL );
						$SQL = "DROP TABLE IF EXISTS " . CLICK_FRAUD_TABLE;
						$iSQL->run ( $SQL );
						/* $SQL = "DROP TABLE IF EXISTS " . CLICK_DEBUG_TABLE;
							$iSQL->run ( $SQL );*/
						$SQL = "DROP TABLE IF EXISTS " . LOG_TABLE;
						$iSQL->run ( $SQL );
						/*Added by Akhilesh on 2/2/2011 for merge iweb_source_log table*/
						/*$SQL = "DROP TABLE IF EXISTS ". 'iweb_source_log';
															$iSQL->run ( $SQL );*/
						/*End*/
						/* Modified by Akhilesh on 19th Nov 2010 to solve merge problem. Unsigned was added for first column
						12/7/2010 for subid*/
						if ( $cl ) {
								$SQL = "
                CREATE TABLE $click_log (
                  hit_id int(10) unsigned NOT NULL,
                  link_id int(11) NOT NULL default '0',
                  word varchar(225) NOT NULL default '',
                  ppc_engine varchar(40) NOT NULL default 'internal',
                  ppc_commission varchar(10) default NULL,
                  gross_price decimal(5,3) NOT NULL default '0.000',
                  price decimal(5,3) NOT NULL default '0.000',
                  hit_date int(11) NOT NULL default '0',
                  hit_time int(11) NOT NULL default '0',
                  affiliate varchar(50) NOT NULL default '',
                  account varchar(50) NOT NULL default '',
                  commission varchar(10) default NULL,
                  paid_flag char(3) default NULL,
                  roi_code int(11) NOT NULL default '0',
                  roi_sale int(11) default NULL,
                  debug_info varchar(100) default NULL,
                  ip varchar(15) default NULL,
                  referer varchar(200) default NULL,
                  user_agent varchar(200) default NULL,
				   subid varchar(50) default NULL,
				    ad_id int(11) default NULL,
					 group_id int(11) default NULL,
                      country varchar(100) default NULL,
                  PRIMARY KEY (hit_id),
                  KEY jhcl_date(hit_date),
                  KEY jhcl_src(hit_date,ppc_engine),
                  KEY jhcl_id(hit_date,link_id),
                  KEY jhcl_acct(hit_date,account),
                  KEY jhcl_aff(hit_date,affiliate),
                  KEY jhcl_word(hit_date,word),
                  KEY jhcl_camp_id(link_id),
                  KEY jhcl_group_id(group_id),
                  KEY jhcl_ad_id(ad_id)
                )
                 ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=(" . $cl . ")
            ";
								$iSQL->run ( $SQL );
						}
						/* Modified by Akhilesh on 19th Nov 2010 to solve merge problem. Unsigned was added for first column.
						12/7/2010 for subid*/
						if ( $cf ) {
								$SQL = "
                CREATE TABLE $click_fraud (
                 hit_id int(10) unsigned NOT NULL,
				 click_tstamp int(11) NOT NULL default '0',
				 fraud_id int(11) NOT NULL default '0',
				  link_id int(11) NOT NULL default '0',
				   word varchar(225) NOT NULL default '',
				    price decimal(5,3) NOT NULL default '0.000',
					hit_date int(11) NOT NULL default '0',
					affiliate varchar(50) NOT NULL default '',
					account varchar(50) NOT NULL default '',
					ppc_engine varchar(40) NOT NULL default 'internal',
					ip_number varchar(20) default NULL,
					user_agent varchar(100) default NULL,
					referer varchar(200) default NULL,
					 debug_info varchar(100) default NULL,
					  fraud_reason varchar(200) default NULL,
					   subid varchar(50) default NULL,
					   `ad_id` int(11) default NULL,
  `group_id` int(11) default NULL, country varchar(100) default NULL, PRIMARY KEY (hit_id), KEY jhfl_hitid(fraud_id), KEY jhfl_date(hit_date), KEY jhfl_src(hit_date,ppc_engine), KEY jhfl_id(hit_date,link_id), KEY jhfl_acct(hit_date,account), KEY jhfl_aff(hit_date,affiliate), KEY jhfl_word(hit_date,word)
                ) ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=(" . $cf . ")
            ";
								$iSQL->run ( $SQL );
						}
						/* Modified by Akhilesh on 12/7/2010 for subid column*/
						if ( $dbg ) {
								$SQL = "
                CREATE TABLE $click_debug (
                  cid varchar(50) NOT NULL default '',
                  click_date int(11) NOT NULL default '0',
                  fraud int(11) NOT NULL default '0',
                  keyword varchar(150) default NULL,
                  price decimal(9,3) default NULL,
                  affiliate varchar(80) NOT NULL default '',
                  ip varchar(15) default NULL,
                  referer varchar(200) default NULL,
                  user_agent varchar(200) default NULL,
                  link_id int(11) default NULL,
                  account varchar(80) default NULL,
                  debug_text text,
                  PRIMARY KEY (cid),
                  KEY jd1(click_date),
                  KEY jd2(click_date,account),
                  KEY jd3(click_date,affiliate),
                  KEY jd4(click_date,link_id)
                )
                TYPE=MERGE UNION=(" . $dbg . ")
            ";
								$iSQL->run ( $SQL );
						}
						/* Modified by Akhilesh on 30th Nov 2010 to add subid,affiliate column. On 12/8/2010 to add search_time.*/
						if ( $logs ) {
								$SQL = "
                CREATE TABLE $log_table (
                  `logdate` int(11) NOT NULL default '0',
				  `search_type` char(120) NOT NULL default '',
				  `criteria` char(120) NOT NULL default '',
				  `ip_number` char(20) NOT NULL default '',
				  `user_agent` char(100) NOT NULL default '',
				  `referer` char(255) NOT NULL default '',
				  `num_results` int(11) default NULL,
				`affiliate` VARCHAR(255) NULL DEFAULT NULL,
				  `subid` varchar(255) default NULL,
				  `search_time` int(11) NOT NULL default '0',
				  KEY `hslog1` (`logdate`),
				  KEY `hslog2` (`search_type`),
				  KEY `hslog3` (`criteria`),
				  KEY `hslog4` (`ip_number`),
				  KEY `hslog5` (`user_agent`),
				  KEY `hslog6` (`referer`),
				 KEY hslog7 (affiliate),
				  KEY hslog8 (subid)
                )
				ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=(" . $logs . ")
              ";
								$iSQL->run ( $SQL );
						}
						/*Added by Akhilesh on 2/2/2011 for merge iweb_source_log table*/
						/* $SQL = "
												CREATE TABLE `iweb_source_log` (
	`id` INT(11) NOT NULL AUTO_INCREMENT,
	`logid` INT(11) NULL DEFAULT NULL,
	`source` VARCHAR(150) NOT NULL,
	`words` VARCHAR(255) NULL DEFAULT NULL,
	`num_results` INT(11) NULL DEFAULT NULL,
	`affiliates` VARCHAR(150) NULL DEFAULT NULL,
	`subid` VARCHAR(150) NULL DEFAULT NULL,
	`searchdate` VARCHAR(8) NULL DEFAULT NULL,
	`searchtime` INT(11) NULL DEFAULT NULL,
	INDEX `id` (`id`),
	INDEX `source` (`source`),
	INDEX `affiliates` (`affiliates`),
	INDEX `subid` (`subid`),
	INDEX `searchdate` (`searchdate`),
	INDEX `searchtime` (`searchtime`)
)
												TYPE=MERGE UNION=($slogs)
								";

								$iSQL->run ( $SQL );*/
						/*End*/
				}
				$header .= "<b>Define Jackhammer Merge Tables</b><br /><br />";
				$click_log_search   = $click_log . "_2%";
				$click_fraud_search = $click_fraud . "_2%";
				$click_debug_search = $click_debug . "_2%";
				$log_table_search   = $log_table . "_2%";
				//$source_table_search = $source_table . "_2%";
				$ctSQL           = "SHOW CREATE TABLE " . CLICK_LOG_TABLE;
				$jh_cl_ct        = $iSQL->search ( array ( "SQL" => $ctSQL , "Return" => "fullhash" ) );
				$jh_cl_def       = $jh_cl_ct[ $click_log ];
				$jh_cl_tablelist = "None Assigned";
				preg_match ( "/(.*)UNION\=\((.*)\)/i" , $jh_cl_def , $m );
				$jh_cl_tablelist = $m[ 2 ];
				$cfSQL           = "SHOW CREATE TABLE " . CLICK_FRAUD_TABLE;
				$jh_cf_ct        = $iSQL->search ( array ( "SQL" => $cfSQL , "Return" => "fullhash" ) );
				$jh_cf_def       = $jh_cf_ct[ $click_fraud ];
				$jh_cf_tablelist = "None Assigned";
				preg_match ( "/(.*)UNION\=\((.*)\)/i" , $jh_cf_def , $m );
				$jh_cf_tablelist = $m[ 2 ];
				/*$dbgSQL = "SHOW CREATE TABLE " . CLICK_DEBUG_TABLE;
				$jh_dbg_ct = $iSQL->search( array("SQL"=>$dbgSQL, "Return"=>"fullhash") );
				$jh_dbg_def = $jh_dbg_ct[$click_debug];
				$jh_dbg_tablelist = "None Assigned";
				preg_match("/(.*)UNION\=\((.*)\)/i", $jh_dbg_def, $m);
				$jh_dbg_tablelist = $m[2];*/
				$logSQL           = "SHOW CREATE TABLE " . LOG_TABLE;
				$jh_log_ct        = $iSQL->search ( array ( "SQL" => $logSQL , "Return" => "fullhash" ) );
				$jh_log_def       = $jh_log_ct[ $log_table ];
				$jh_log_tablelist = "None Assigned";
				preg_match ( "/(.*)UNION\=\((.*)\)/i" , $jh_log_def , $m );
				$jh_log_tablelist = $m[ 2 ];
				/*Added by Akhilesh on 2/2/2011 for merge iweb_source_log table*/
				/*  $slogSQL = "SHOW CREATE TABLE iweb_source_log";
						$jh_slog_ct = $iSQL->search( array("SQL"=>$slogSQL, "Return"=>"fullhash") );
						$jh_slog_def = $jh_slog_ct[$source_table];
						$jh_slog_tablelist = "None Assigned";
						preg_match("/(.*)UNION\=\((.*)\)/i", $jh_slog_def, $m);
						$jh_slog_tablelist = $m[2];*/
				/*End*/
				$mrg_jhcl_SQL         = "SHOW TABLES LIKE '$click_log_search'";
				$cl_log_tables        = $iSQL->search ( array ( "SQL" => $mrg_jhcl_SQL , "Return" => "array" ) );
				$jh_click_log_options = '';
				foreach ( $cl_log_tables as $tbl ) {
						$checked = '';
						if ( preg_match ( "/" . $tbl . "/i" , $jh_cl_tablelist ) ) {
								$checked = "checked";
						}
						$jh_click_log_options .= " <input type='checkbox' name='jh_click_log_assignments[]' value='$tbl' $checked />$tbl<br /> ";
				}
				$mrg_jhcf_SQL           = "SHOW TABLES LIKE '$click_fraud_search'";
				$fraud_tables           = $iSQL->search ( array ( "SQL" => $mrg_jhcf_SQL , "Return" => "array" ) );
				$jh_click_fraud_options = '';
				foreach ( $fraud_tables as $tbl ) {
						$checked = '';
						if ( preg_match ( "/" . $tbl . "/i" , $jh_cf_tablelist ) ) {
								$checked = "checked";
						}
						$jh_click_fraud_options .= " <input type='checkbox' name='jh_click_fraud_assignments[]' value='$tbl' $checked />$tbl<br /> ";
				}
				/* $mrg_jhdbg_SQL = "SHOW TABLES LIKE '$click_debug_search'";
					$debug_tables = $iSQL->search( array("SQL"=>$mrg_jhdbg_SQL, "Return"=>"array") );
					$jh_debug_options = '';
					foreach ( $debug_tables as $tbl ) {
									$checked = '';
									if ( preg_match("/".$tbl."/i", $jh_dbg_tablelist) ) { $checked = "checked"; }
									$jh_debug_options .= " <input type='checkbox' name='jh_debug_assignments[]' value='$tbl' $checked />$tbl<br /> ";
					}*/
				$mrg_log_SQL = "SHOW TABLES LIKE '$log_table_search'";
				$log_tables  = $iSQL->search ( array ( "SQL" => $mrg_log_SQL , "Return" => "array" ) );
				$log_options = '';
				foreach ( $log_tables as $tbl ) {
						$checked = '';
						if ( preg_match ( "/" . $tbl . "/i" , $jh_log_tablelist ) ) {
								$checked = "checked";
						}
						$log_options .= " <input type='checkbox' name='hyperseek_log_assignments[]' value='$tbl' $checked />$tbl<br /> ";
				}
				/*Added by Akhilesh on 2/2/2011 for merge iweb_source_log table*/
				/* $mrg_slog_SQL = "SHOW TABLES LIKE '$source_table_search'";
					$slog_tables = $iSQL->search( array("SQL"=>$mrg_slog_SQL, "Return"=>"array") );
					$slog_options = '';
					foreach ( $slog_tables as $tbl ) {
									$checked = '';
									if ( preg_match("/".$tbl."/i", $jh_slog_tablelist) ) { $checked = "checked"; }
									$slog_options .= " <input type='checkbox' name='source_log_assignments[]' value='$tbl' $checked />$tbl<br /> ";
					}*/
				/*End*/
				$output .= "
            $header
            Current Assignments:<br />
            <form action='$PROG_URL' method='post'>
            <div style='padding-left:10px'>
             <table cellspacing='0' cellpadding='10'>
               <tr style='background:black;'>
                 <th style='color:white'>jh_click_log</th>
                 <th style='color:white'>jh_click_fraud</th>
                 <th style='color:white'>jh_debug</th>
                 <th style='color:white'>hyperseek_log</th>
				 <!-- <th style='color:white'>source_log</th> -->
               </tr>
               <tr style='background:#e0e0e0'>
                 <td valign='top' style='border:1px solid black'>$jh_click_log_options</td>
                 <td valign='top' style='border:1px solid black'>$jh_click_fraud_options</td>
                 <td valign='top' style='border:1px solid black'>$jh_debug_options</td>
                 <td valign='top' style='border:1px solid black'>$log_options</td>
				 <!-- <td valign='top' style='border:1px solid black'>$slog_options</td> -->
               </tr>
               <tr>
                   <td colspan='4' align='center'>
                      <input type='hidden' name='action' value='merge_tables' />
                      <input type='hidden' name='admin' value='hyperseek' />
                      <input type='hidden' name='save_merge_assignments' value='1' />
                      <input type='submit' value='Save Assignments' />
                   </td>
               </tr>
            </div>
            </form>
       ";
				admin_output ( "Merge Table Management" , $output );
		}

		function setup_merge_tables ()
		{
				global $iSQL;
				global $config;
				global $data_dir;
				echo "<pre>";
				echo "Creating Merge Tables\n";
				$merged = array ();
				// Table Vars
				$click_log   = CLICK_LOG_TABLE;
				$click_fraud = CLICK_FRAUD_TABLE;
				$click_debug = CLICK_DEBUG_TABLE;
				$log_table   = LOG_TABLE;
				$SQL         = "SELECT distinct hit_date FROM " . CLICK_LOG_TABLE;
				$dates       = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
				foreach ( $dates as $date ) {
						$date            = substr ( $date , 0 , 6 );
						$merged[ $date ] = 1;
				}
				$jh_cl  = array ();
				$jh_cf  = array ();
				$jh_dbg = array ();
				foreach ( $merged as $date => $val ) {
						echo "Date: $date\n";
						create_month_tables ( $date );
						array_push ( $jh_cl , CLICK_LOG_TABLE . "_" . $date );
						array_push ( $jh_cf , CLICK_FRAUD_TABLE . "_" . $date );
						array_push ( $jh_dbg , CLICK_DEBUG_TABLE . "_" . $date );
				}
				echo "\n\n";
				echo "Loading Data\n\n";
				reset ( $dates );
				foreach ( $dates as $date ) {
						echo "$date ... $target\n";
						$target = substr ( $date , 0 , 6 );
						merge_tables_move_data ( $date , $target );
				}
				echo "\n\n";
				$jh_click_log_tables     = join ( "," , $jh_cl );
				$jh_click_fraud_tables   = join ( "," , $jh_cf );
				$jackhammer_debug_tables = join ( "," , $jh_dbg );
				create_merge_tables ( $jh_click_log_tables , $jh_click_fraud_tables , $jackhammer_debug_tables );
				// Turn this option on, and re-save the options //
				$settings                            = '';
				$file                                = "$data_dir/global.config";
				$config[ 'jackhammer_merge_tables' ] = "YES";
				foreach ( $config as $var => $val ) {
						$settings .= $var . "=" . $val . "\n";
				}
				copy ( $file , $file . '.save' );
				list ( $status , $message ) = write_file ( $file , $settings );
				if ( file_exists ( "$data_dir/phpsettings.inc" ) ) {
						unlink ( "$data_dir/phpsettings.inc" );
				}
				echo "\n\nCOMPLETE\n\n";
				exit;
		}


		function merge_tables_move_data ( $date , $tgt )
		{
				global $iSQL;
				// Table Vars
				$jh_click_log     = CLICK_LOG_TABLE . "_" . $tgt;
				$jh_click_fraud   = CLICK_FRAUD_TABLE . "_" . $tgt;
				$jackhammer_debug = CLICK_DEBUG_TABLE . "_" . $tgt;
				$SQL              = "INSERT INTO $jh_click_log SELECT * FROM " . CLICK_LOG_TABLE . " WHERE hit_date = $date";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_log Loaded\n";
				} else {
						echo "Error loading $jh_click_log\n";
				}
				$SQL = "INSERT INTO $jh_click_fraud SELECT * FROM " . CLICK_FRAUD_TABLE . " WHERE hit_date = $date";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_fraud Loaded\n";
				} else {
						echo "Error loading $jh_click_fraud\n";
				}
				$SQL = "INSERT INTO $jackhammer_debug SELECT * FROM " . CLICK_DEBUG_TABLE . " WHERE click_date = $date";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jackhammer_debug Loaded\n";
				} else {
						echo "Error loading $jackhammer_debug\n";
				}
		}

		function create_month_tables ( $date )
		{
				global $iSQL;
				$jh_click_log     = CLICK_LOG_TABLE . "_" . $date;
				$jh_click_fraud   = CLICK_FRAUD_TABLE . "_" . $date;
				$jackhammer_debug = CLICK_DEBUG_TABLE . "_" . $date;
				$SQL              = "DROP TABLE IF EXISTS $jh_click_log";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_log Dropped\n";
				} else {
						echo "Error dropping $jh_click_log\n";
				}
				$SQL = "DROP TABLE IF EXISTS $jh_click_fraud";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_fraud Dropped\n";
				} else {
						echo "Error dropping $jh_click_fraud\n";
				}
				$SQL = "DROP TABLE IF EXISTS $jackhammer_debug";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jackhammer_debug Dropped\n";
				} else {
						echo "Error dropping $jackhammer_debug\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE IF NOT EXISTS $jh_click_log (
              hit_id int(10) UNSIGNED NOT NULL auto_increment,
              link_id int(11) NOT NULL default '0',
              word varchar(225) NOT NULL default '',
              ppc_engine varchar(40) NOT NULL default 'internal',
              ppc_commission varchar(10) default NULL,
              gross_price decimal(5,3) NOT NULL default '0.000',
              price decimal(5,3) NOT NULL default '0.000',
              hit_date int(11) NOT NULL default '0',
              hit_time int(11) NOT NULL default '0',
              affiliate varchar(50) NOT NULL default '',
              account varchar(50) NOT NULL default '',
              commission varchar(10) default NULL,
              paid_flag char(3) default NULL,
              roi_code int(11) NOT NULL default '0',
              roi_sale int(11) default NULL,
              debug_info varchar(100) default NULL,
              ip varchar(15) default NULL,
              referer varchar(200) default NULL,
              user_agent varchar(200) default NULL,
			   subid varchar(50) default NULL,
				    ad_id int(50) default NULL,
					 group_id int(50) default NULL,
              PRIMARY KEY (hit_id),
              KEY jhcl_date(hit_date),
              KEY jhcl_src(hit_date,ppc_engine),
              KEY jhcl_id(hit_date,link_id),
              KEY jhcl_acct(hit_date,account),
              KEY jhcl_aff(hit_date,affiliate),
              KEY jhcl_word(hit_date,word),
			   KEY `jhcl_camp_id` (`link_id`),
				  KEY `jhcl_group_id` (`group_id`),
				  KEY `jhcl_ad_id` (`ad_id`)
            )
        ";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_log Created\n";
				} else {
						echo "Error creating $jh_click_log\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE IF NOT EXISTS $jh_click_fraud (
              hit_id int(10) UNSIGNED NOT NULL auto_increment,
              click_tstamp int(11) NOT NULL default '0',
              fraud_id int(11) NOT NULL default '0',
              link_id int(11) NOT NULL default '0',
              word varchar(225) NOT NULL default '',
              price decimal(5,3) NOT NULL default '0.000',
              hit_date int(11) NOT NULL default '0',
              affiliate varchar(50) NOT NULL default '',
              account varchar(50) NOT NULL default '',
              ppc_engine varchar(40) NOT NULL default 'internal',
              ip_number varchar(20) default NULL,
              user_agent varchar(100) default NULL,
              referer varchar(200) default NULL,
              debug_info varchar(100) default NULL,
              fraud_reason varchar(200) default NULL,
			   subid varchar(50) default NULL,
				  ad_id int(11) default NULL,
				  group_id int(11) default NULL,
              PRIMARY KEY (hit_id),
              KEY jhfl_hitid(fraud_id),
              KEY jhfl_date(hit_date),
              KEY jhfl_src(hit_date,ppc_engine),
              KEY jhfl_id(hit_date,link_id),
              KEY jhfl_acct(hit_date,account),
              KEY jhfl_aff(hit_date,affiliate),
              KEY jhfl_word(hit_date,word)
            )
        ";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_fraud Created\n";
				} else {
						echo "Error creating $jh_click_fraud\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE IF NOT EXISTS $jackhammer_debug (
              cid varchar(50) NOT NULL default '',
              click_date int(11) NOT NULL default '0',
              fraud int(11) NOT NULL default '0',
              keyword varchar(150) default NULL,
              price decimal(9,3) default NULL,
              affiliate varchar(80) NOT NULL default '',
              ip varchar(15) default NULL,
              referer varchar(200) default NULL,
              user_agent varchar(200) default NULL,
              link_id int(11) default NULL,
              account varchar(80) default NULL,
              debug_text text,
              PRIMARY KEY (cid),
              KEY jd1(click_date),
              KEY jd2(click_date,account),
              KEY jd3(click_date,affiliate),
              KEY jd4(click_date,link_id)
            )
        ";
				$iSQL->run ( $SQL );
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jackhammer_debug Created\n";
				} else {
						echo "Error creating $jackhammer_debug\n";
				}
		}

		function create_merge_tables ( $jh_click_log_tables , $jh_click_fraud_tables , $jh_debug_tables )
		{
				global $iSQL;
				$jh_click_log     = CLICK_LOG_TABLE;
				$jh_click_fraud   = CLICK_FRAUD_TABLE;
				$jackhammer_debug = CLICK_DEBUG_TABLE;
				$SQL              = "DROP TABLE IF EXISTS " . CLICK_LOG_TABLE;
				if ( $iSQL->run ( $SQL ) ) {
						echo "iweb_jh_click_log Dropped\n";
				} else {
						echo "Error dropping iweb_jh_click_log\n";
				}
				$SQL = "DROP TABLE IF EXISTS " . CLICK_FRAUD_TABLE;
				if ( $iSQL->run ( $SQL ) ) {
						echo "iweb_jh_click_fraud Dropped\n";
				} else {
						echo "Error dropping iweb_jh_click_fraud\n";
				}
				$SQL = "DROP TABLE IF EXISTS " . CLICK_DEBUG_TABLE;
				if ( $iSQL->run ( $SQL ) ) {
						echo "iweb_jackhammer_debug Dropped\n";
				} else {
						echo "Error dropping iweb_jackhammer_debug\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE $jh_click_log (
              hit_id int(10) unsigned NOT NULL auto_increment,
              link_id int(11) NOT NULL default '0',
              word varchar(225) NOT NULL default '',
              ppc_engine varchar(40) NOT NULL default 'internal',
              ppc_commission varchar(10) default NULL,
              gross_price decimal(5,3) NOT NULL default '0.000',
              price decimal(5,3) NOT NULL default '0.000',
              hit_date int(11) NOT NULL default '0',
              hit_time int(11) NOT NULL default '0',
              affiliate varchar(50) NOT NULL default '',
              account varchar(50) NOT NULL default '',
              commission varchar(10) default NULL,
              paid_flag char(3) default NULL,
              roi_code int(11) NOT NULL default '0',
              roi_sale int(11) default NULL,
              debug_info varchar(100) default NULL,
              ip varchar(15) default NULL,
              referer varchar(200) default NULL,
              user_agent varchar(200) default NULL,
			   subid varchar(50) default NULL,
				    ad_id int(50) default NULL,
					 group_id int(50) default NULL,
              PRIMARY KEY (hit_id),
              KEY jhcl_date(hit_date),
              KEY jhcl_src(hit_date,ppc_engine),
              KEY jhcl_id(hit_date,link_id),
              KEY jhcl_acct(hit_date,account),
              KEY jhcl_aff(hit_date,affiliate),
              KEY jhcl_word(hit_date,word),
			   KEY `jhcl_camp_id` (`link_id`),
				  KEY `jhcl_group_id` (`group_id`),
				  KEY `jhcl_ad_id` (`ad_id`)
            )
			ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=($jh_click_log_tables)

        ";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_log Merge Created\n";
				} else {
						echo "Error creating $jh_click_log Merge\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE $jh_click_fraud (
              hit_id int(10) UNSIGNED NOT NULL,
              click_tstamp int(11) NOT NULL default '0',
              fraud_id int(11) NOT NULL default '0',
              link_id int(11) NOT NULL default '0',
              word varchar(225) NOT NULL default '',
              price decimal(5,3) NOT NULL default '0.000',
              hit_date int(11) NOT NULL default '0',
              affiliate varchar(50) NOT NULL default '',
              account varchar(50) NOT NULL default '',
              ppc_engine varchar(40) NOT NULL default 'internal',
              ip_number varchar(20) default NULL,
              user_agent varchar(100) default NULL,
              referer varchar(200) default NULL,
              debug_info varchar(100) default NULL,
              fraud_reason varchar(200) default NULL,
			  subid varchar(50) default NULL,
				  ad_id int(11) default NULL,
				  group_id int(11) default NULL,
              PRIMARY KEY (hit_id),
              KEY jhfl_hitid(fraud_id),
              KEY jhfl_date(hit_date),
              KEY jhfl_src(hit_date,ppc_engine),
              KEY jhfl_id(hit_date,link_id),
              KEY jhfl_acct(hit_date,account),
              KEY jhfl_aff(hit_date,affiliate),
              KEY jhfl_word(hit_date,word)
            )
			ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=($jh_click_fraud_tables)

        ";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jh_click_fraud Merge Created\n";
				} else {
						echo "Error creating $jh_click_fraud Merge\n";
				}
				/* Modified by Akhilesh on 12/7/2010 to add subid column*/
				$SQL = "
            CREATE TABLE $jackhammer_debug (
              cid varchar(50) NOT NULL default '',
              click_date int(11) NOT NULL default '0',
              fraud int(11) NOT NULL default '0',
              keyword varchar(150) default NULL,
              price decimal(9,3) default NULL,
              affiliate varchar(80) NOT NULL default '',
              ip varchar(15) default NULL,
              referer varchar(200) default NULL,
              user_agent varchar(200) default NULL,
              link_id int(11) default NULL,
              account varchar(80) default NULL,
              debug_text text,
              PRIMARY KEY (cid),
              KEY jd1(click_date),
              KEY jd2(click_date,account),
              KEY jd3(click_date,affiliate),
              KEY jd4(click_date,link_id)
            )
			ENGINE=MRG_MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci UNION=($jh_debug_tables)

        ";
				if ( $iSQL->run ( $SQL ) ) {
						echo "$jackhammer_debug Merge Created\n";
				} else {
						echo "Error creating $jackhammer_debug Merge\n";
				}
		}

?>
