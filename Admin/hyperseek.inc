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

		class hyperseekAdmin
		{

				var $output;

				function hyperseekAdmin ()
				{
						// Includes & Setup
						include_once ( "dsX/iSQL/Record/Listing.inc" );
						include_once ( "dsX/iSQL/Record/Keyword.inc" );
						include_once ( "dsX/iSQL/Record/CategoryNavigator.inc" );
						include_once ( "dsX/Admin/category-functions.inc" );
						include_once ( "dsX/Admin/hyperseek-crons.inc" );
						include_once ( "dsX/Admin/jackhammer.inc" );
						include_once ( "dsX/Admin/reports.inc" );
						include_once ( "dsX/Admin/affiliate-reports.inc" );
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

				function check_duplicates ( $url = '' )
				{
						global $config;
						global $oSQL;
						$urlinfo = parse_urL ( $url );
						$url     = preg_replace ( "/\'/" , "''" , $url );
						$host    = $urlinfo[ 'host' ];
						if ( $config[ 'yh_add_dups' ] == "STRICT" ) {
								$SQL = "SELECT count(*) FROM " . LINKS_TABLE . " WHERE url = '$url'";
						} else {
								$SQL = "SELECT count(*) FROM " . LINKS_TABLE . " WHERE url LIKE '%$host%'";
						}
						$dups = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );

						return array ( $dups , $host );
				}

				function submissions ()
				{
						global $language;
						global $oSQL;
						global $config;
						global $CGI;
						global $SCRIPT_NAME;
						global $PPCMANAGER;
						global $my_accounts;
						$accounts_and = '';
						if ( count ( $my_accounts ) >= 1 ) {
								$list         = join ( "','" , array_keys ( $my_accounts ) );
								$accounts_and = " AND account IN('$list')";
						}
						// Force a bad query if this is a ppc manager doing the lookup, and they don't have any accounts
						if ( $PPCMANAGER && !$accounts_and ) {
								$accounts_and = "AND 1=2";
						}
						$list        = '';
						$SQL         = "SELECT * FROM " . TEMP_TABLE . " WHERE submission_type = '$CGI[type]' $accounts_and";
						$submissions = $oSQL->search ( array ( "SQL" => $SQL ) );
						$x           = 0;
						$qualifiers  = qualifier_list ();
						$start       = $CGI[ start ];
						if ( $start <= 1 || empty( $start ) ) {
								$start = 0;
						}
						$max               = $start + $config[ yh_max_new ];
						$total_submissions = count ( $submissions );
						if ( $total_submissions < $max ) {
								$max = $total_submissions;
						}
						foreach ( $submissions as $submission ) {
								if ( $moderator ) {
										if ( !preg_match ( "/y/i" , $moderator_categories[ $submission[ category ] ][ can_approve_listings ] ) ) {
												continue;
										}
								}
								$id_array .= " ids[$x] = $submission[id]; \n";
								$x++;
								if ( $x < $start ) {
										continue;
								}
								if ( $x > $max ) {
										break;
								}
								list ( $num_duplicates , $root ) = $this->check_duplicates ( $submission[ url ] );
								$duplicates = "&nbsp;";
								if ( $num_duplicates >= 1 ) {
										$duplicates = " <b>$num_duplicates</b> <a href='$SCRIPT_NAME?admin=hyperseek&action=search&Terms=url:$root'>Dups</a> ";
								}
								$styles .= "#x$submission[id] {position:relative;}\n";
								$list .= "
                  <tr id='x$submission[id]' style='background:white'>
                    <td><b>$submission[title]</b></td>
                    <td><a href='javascript:changeText($submission[id]);'>Review</a></td>
                    <td><a href='javascript:approve($submission[id]);'>Approve</a></td>
                    <td><a href='javascript:reject($submission[id]);'>Reject</a></td>
                    <td><a href='javascript:reset($submission[id]);'>Reset</a></td>
                    <td>$duplicates</td>
                  </tr>
              ";
								foreach ( $submission as $key => $val ) {
										$masterkeys[ $key ]++;
										$submission[ $key ] = preg_replace ( "/\r|\n|\cM/" , "" , $submission[ $key ] );
										$submission[ $key ] = preg_replace ( "/\\\'/" , "'" , $submission[ $key ] );
										$submission[ $key ] = preg_replace ( "/\\\"/" , '"' , $submission[ $key ] );
										$submission[ $key ] = preg_replace ( "/\'/" , "\\\'" , $submission[ $key ] );
										$submission[ $key ] = preg_replace ( "/\"/" , "\"" , $submission[ $key ] );
										$jsarray .= " $key" . "[" . $submission[ id ] . "] = '$submission[$key]'; \n";
										if ( $key == "category" ) {
												$SQL     = "SELECT full_name FROM " . CATEGORY_TABLE . " WHERE category_id = '$submission[$key]'";
												$catname = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
												$catname = preg_replace ( "/\'/" , "\\'" , $catname );
												$jsarray .= " category_name" . "[" . $submission[ id ] . "] = '$catname'; \n";
										}
								}
								$jsarray .= " email_template" . "[" . $submission[ id ] . "] = ''; \n";
								$jsarray .= " link_action" . "[" . $submission[ id ] . "] = ''; \n";
						}
						$other_fields = '';
						foreach ( $masterkeys as $key => $val ) {
								$arrays .= "       var $key = new Array;\n";
								$update_ie .= " $key" . "[current_id] = document.input_form.$key.value; \n";
								$ie_change .= " document.input_form.$key.value = $key" . "[current_id];\n";
								//if ( $key == "max_limit" ) { $ie_change .= " alert('Changing $key to' + $key" . "[current_id])\n"; }
								$update_ns .= " $key" . "[current_id] = document.divMessage.document.input_form.$key.value; \n";
								$ns_change .= " document.divMessage.document.input_form.$key.value = $key" . "[current_id]; \n";
								if ( preg_match ( "/field/i" , $key ) ) {
										list ( $junk , $fieldnum ) = preg_split ( "/field/" , $key );
										if ( $fieldnum > 10 ) {
												$other_fields .= "<tr><td>&nbsp;</td><td>&nbsp;</td> <td> $key </td><td> <input name='$key' value=''></td></tr>";
										}
								}
						}
						// Be able to show the category name in the catname div //
						$arrays .= "       var category_name = new Array;\n";
						$ie_change .= " document.all['catname'].innerHTML = category_name[current_id] \n";
						## List of possible emails to send.
						$emails  = '';
						$SQL     = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'EMAIL'";
						$letters = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $letters as $email ) {
								$emails .= " <option value='$email[template_name]'>$email[template_name]</option> ";
						}
						$form  = "
             <form name='input_form'>
             <table border=1 bgcolor='black' bordercolor='black' cellspacing=1 cellpadding=0><tr><td>
               <table bgcolor='white' width='100%' cellspacing=0 cellpadding=5 border=0>
                   <tr bgcolor='black'>
                     <td colspan=4>
                      <font color='white'><b>$language[queue_edit_entry]</b></font>
                     </td>
                   </tr>

                   <tr>
                     <td>$language[queue_title]</td>
                     <td colspan=3><input name='title' value=''></td>
                   </tr>

                   <tr>
                     <td><a href='javascript:visitLink()'>$language[queue_url]</a></td>
                     <td colspan=3><input name='url' value=''></td>
                   </tr>

                   <tr>
                     <td colspan=4>
                       $language[queue_description]<br />
                       &nbsp;&nbsp;&nbsp;<textarea cols=40 rows=3 name='description'></textarea>
                     </td>
                   </tr>

                   <tr>
                     <td colspan=4>
                       $language[queue_keywords]<br />
                       &nbsp;&nbsp;&nbsp;<textarea cols=40 rows=3 name='keywords'></textarea>
                     </td>
                   </tr>

                   <tr>
                     <td style='padding:5px;' colspan=4>
                        <div style='background:#f5f5f5; border:1px solid #e0e0e0; padding:2px;'>
                        <a href='javascript:changeCategory()'>$language[queue_category]</a> <input name='category' value=''>
                        <div style='color:blue' id='catname'></div>
                        </div>
                   </tr>

                   <tr>
                     <td colspan=4>
                        $language[queue_qualifier] <select name='qualifier'>
                         <option value=''>$language[queue_qualifier_none]</option>
                         $qualifiers
                         </select>
                   </tr>

                   <tr>
                      <td valign='top' nowrap colspan=2>
                        <b>contact information</b>
                      </td>
                      <td valign='top' nowrap colspan=2>
                        <b>other information</b>
                      </td>
                   </tr>

                    <tr>
                    <td> $language[queue_account] </td><td> <input name='account' value=''></td>
                    <td> $language[queue_rank] </td><td> <input name='rank' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_name] </td><td> <input name='name' value='' size=15></td>
                    <td> $language[queue_xfactor] </td><td> <input name='xfactor' value=''></td>
                    </tr>

                    <tr>
                    <td> <a href='javascript:emailLink()'>$language[queue_email]</a> </td><td> <input name='email' value='' size=15></td>
                    <td> $language[queue_startdate] </td><td> <input name='startdate' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_address] </td><td> <input name='address' value=''></td>
                    <td> $language[queue_enddate] </td><td> <input name='enddate' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_city] </td><td> <input name='city' size=10 value=''></td>
                    <td> $language[queue_price] </td><td> <input name='price' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_state] </td><td> <input name='state' size=2 value=''></td>
                    <td> <a href='javascript:view_site_icon()'>$language[queue_site_icon] </td><td> <input name='site_icon' value=''></a></td>
                    </tr>

                    <tr>
                    <td> $language[queue_zip] </td><td> <input name='zip' size=8 value=''></td>
                    <td> <a href='javascript:view_desc_image()'>$language[queue_desc_image]</a> </td><td> <input name='desc_image' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_country] </td><td> <input name='country' value=''></td>
                    <td> $language[queue_title_tag] </td><td> <input name='title_tag' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_phone]</td><td> <input name='phone' size=10 value=''></td>
                    <td> $language[queue_template] </td><td> <input name='extra_link' value=''></td>
                    </tr>

                    <tr>
                    <td> $language[queue_fax] </td><td> <input name='fax' size=10 value=''></td>
                    <td> $language[queue_field1] </td><td> <input name='field1' value=''></td>
                    </tr>

                    <tr>
                    <td> adult </td><td> <input name='is_adult' size=10 value=''></td>
                    <td> $language[queue_field2] </td><td> <input name='field2' value=''></td>
                    </tr>

                    <tr>
                    <td> Max Limit </td><td> <input name='max_limit' size=10 value=''></td>
                    <td> $language[queue_field3] </td><td> <input name='field3' value=''></td>
                    </tr>

                    <tr>
                    <td> Max Period </td><td> <input name='max_period' size=10 value=''></td>
                    <td> $language[queue_field4] </td><td> <input name='field4' value=''></td>
                    </tr>

                    <tr>
                    <td> Active? </td><td> <input name='active' size=10 value=''></td>
                    <td> $language[queue_field5] </td><td> <input name='field5' value=''></td>
                    </tr>


                    <tr><td>&nbsp;</td><td>&nbsp;</td> <td> $language[queue_field6] </td><td> <input name='field6' value=''></td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td> <td> $language[queue_field7] </td><td> <input name='field7' value=''></td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td> <td> $language[queue_field8] </td><td> <input name='field8' value=''></td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td> <td> $language[queue_field9] </td><td> <input name='field9' value=''></td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td> <td> $language[queue_field10] </td><td> <input name='field10' value=''></td></tr>
                    $other_fields

                   <tr><td colspan=4>
                      <hr />
                      $language[queue_email_prompt]
                      <select name='email_template'>
                        <option value=''>$language[queue_no_email]</option>
                        $emails
                      </select><br />
                      action to take:
                      <input type='radio' name='link_action' value=''>$language[queue_action_none]
                      <input type='radio' name='link_action' value='add'>$language[queue_action_approve]
                      <input type='radio' name='link_action' value='delete'>$language[queue_action_reject]
                      <p align='right'><a href='javascript:cancelForm();'>$language[queue_action_cancel]</a> <a href='javascript:saveForm();'>$language[queue_action_save]</a></p>

                   </td></tr>
               </table>
             </td></tr></table>

                <input type='hidden' name='id' value=''>
                <input type='hidden' name='submission_type' value=''>
                <input type='hidden' name='submitted_by' value=''>
                <input type='hidden' name='last_status' value=''>
                <input type='hidden' name='rate_score' value=''>
                <input type='hidden' name='rate_votes' value=''>
                <input type='hidden' name='top_votes' value=''>
                <input type='hidden' name='hits' value=''>
                <input type='hidden' name='av_links' value=''>


             </form>
           ";
						$form  = preg_replace ( "/\r|\n|\cM/" , " " , $form );
						$form  = preg_replace ( "/\'/" , "\\\'" , $form );
						$split = "----------";
						$start++;
						$nav = "Showing $start to $max of $total_submissions ... ";
						if ( $total_submissions > $config[ yh_max_new ] ) {
								$numpages = ceil ( $total_submissions / $config[ yh_max_new ] );
								if ( $total_submissions % $config[ yh_max_new ] ) {
										$numpages++;
								}
								$numpages--;
								for ( $x = 0; $x < $numpages; $x++ ) {
										$start = $x * $config[ yh_max_new ] + 1;
										$z     = $x + 1;
										if ( $start == $CGI[ start ] ) {
												$nav .= " $z ";
										} else {
												$nav .= " <a href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=$CGI[type]&start=$start'>$z</a> ";
										}
								}
						}
						$QUEUE = "

              <STYLE>
                 #divMessage {position:absolute; top:20; left:10;}
                 $styles
              </STYLE>

              <SCRIPT LANGUAGE='javascript'>

                  function checkBrowser(){
                    this.ver=navigator.appVersion
                    this.dom=document.getElementById?1:0
                    this.ie5=(this.ver.indexOf('MSIE 5')>-1 && this.dom)?1:0;
                    this.ie6=(this.ver.indexOf('MSIE 6')>-1 && this.dom)?1:0;
                    this.ie7=(this.ver.indexOf('MSIE 7')>-1 && this.dom)?1:0;
                    this.ie4=(document.all && !this.dom)?1:0;
                    this.ns5=(this.dom && parseInt(this.ver) >= 5) ?1:0;
                    this.ns4=(document.layers && !this.dom)?1:0;
                    if ( this.ns5 ) { this.ns4 = 1; }
                    this.bw=( this.ie7 || this.ie6 || this.ie5 || this.ie4 || this.ns4 || this.ns5)
                    return this
                  }

                  bw=new checkBrowser();

                  var i_form = '$form';
                  var current_id;
                  var ids = new Array;
                  var email_template = new Array;
                  var link_action = new Array;

                  $id_array

                  $arrays
                  $jsarray

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
                       document.layers['divMessage'].moveTo(20,10);
                     }
                     else if(bw.dom||bw.ie4) this.writeref.innerHTML=text
                  }


                  function reset(num){
                      link_action[num] = '';
                      var divid = 'x' + num;
                      var tdiv = bw.dom? document.getElementById(divid):bw.ie4?document.all[divid]:bw.ns4?document.layers[divid].document:0;
                      tdiv.style.background='white';
                  }

                  function reject(num){
                      link_action[num] = 'delete';
                      var divid = 'x' + num;
                      var tdiv = bw.dom? document.getElementById(divid):bw.ie4?document.all[divid]:bw.ns4?document.layers[divid].document:0;
                      tdiv.style.background='#FFB6C1';
                  }

                  function approve(num) {
                      link_action[num] = 'add';
                      var divid = 'x' + num;
                      var tdiv = bw.dom? document.getElementById(divid):bw.ie4?document.all[divid]:bw.ns4?document.layers[divid].document:0;
                      tdiv.style.background='#F0F8FF';
                  }

                  //The mouoseover function. Calls the objects writeIt method to write the text to the div.
                  function changeText(num){

                      if(bw.bw) {
                         current_id=num;
                         oMessage.writeIt(i_form);

                         if(bw.ns4) {

                            // Re-select the email template
                            for (var i = 0; i < document.divMessage.document.input_form.email_template.options.length; i++) {
                               var currval = document.divMessage.document.input_form.email_template.options[i].value;
                               if (currval == email_template[current_id] ) {
                                  document.divMessage.document.input_form.email_template.options[i].selected = true;
                                  break;
                               }
                            }

                            // Re-select the qualifier.
                            for (var i = 0; i < document.divMessage.document.input_form.qualifier.options.length; i++) {
                               var currval = document.divMessage.document.input_form.qualifier.options[i].value;
                               if (currval == qualifier[current_id] ) {
                                  document.divMessage.document.input_form.qualifier.options[i].selected = true;
                                  break;
                               }
                            }

                            // Re Check the action
                            document.divMessage.document.input_form.link_action[0].checked = 0;
                            document.divMessage.document.input_form.link_action[1].checked = 0;
                            document.divMessage.document.input_form.link_action[2].checked = 0;
                            if ( link_action[current_id] == 'add' ) { document.divMessage.document.input_form.link_action[1].checked = true; }
                            if ( link_action[current_id] == 'delete' ) { document.divMessage.document.input_form.link_action[2].checked = true; }

                            $ns_change
                         }
                         else {

                            // Re-select the email template
                            for (var i = 0; i < document.input_form.email_template.options.length; i++) {
                               var currval = document.input_form.email_template.options[i].value;
                               if (currval == email_template[current_id] ) {
                                  document.input_form.email_template.options[i].selected = true;
                                  break;
                               }
                            }

                            // Re-select the qualifier
                            for (var i = 0; i < document.input_form.qualifier.options.length; i++) {
                               var currval = document.input_form.qualifier.options[i].value;
                               if (currval == qualifier[current_id] ) {
                                  document.input_form.qualifier.options[i].selected = true;
                                  break;
                               }
                            }

                            // Re Check the action
                            document.input_form.link_action[0].checked = 0;
                            document.input_form.link_action[1].checked = 0;
                            document.input_form.link_action[2].checked = 0;
                            if ( link_action[current_id] == 'add' ) { document.input_form.link_action[1].checked = true;  }
                            if ( link_action[current_id] == 'delete' ) { document.input_form.link_action[2].checked = true; }


                            $ie_change
                         }
                      }

                  }

                  //The init function. Calls the object constructor and set some properties.
                  function changeTextInit(){
                     if(bw.bw){
                       oMessage=new makeChangeTextObj('divMessage');
                    }
                  }

                  function closeForm() {
                    if(bw.bw) oMessage.writeIt('');
                    if(bw.ns4) { document.layers['divMessage'].moveTo(1000,1000) }
                    document.submission_form.holder.focus();
                  }

                  function cancelForm() {
                     closeForm();
                  }

                  function saveForm() {

                     var divid = 'x' + current_id;
                     tdiv = bw.dom? document.getElementById(divid):bw.ie4?document.all[divid]:bw.ns4?document.layers[divid].document:0;

                     if(bw.ns4) {
                        $update_ns
                        // Save the email template.
                        var idx = document.divMessage.document.input_form.email_template.selectedIndex;
                        email_template[current_id] = document.divMessage.document.input_form.email_template.options[idx].value;

                        // Save the qualifier.
                        var qidx = document.divMessage.document.input_form.qualifier.selectedIndex;
                        qualifier[current_id] = document.divMessage.document.input_form.qualifier.options[qidx].value;

                        // Save the desired action.
                        for (var i=0; i<3; i++) {
                            if (document.divMessage.document.input_form.link_action[i].checked=='1') {
                                link_action[current_id] = document.divMessage.document.input_form.link_action[i].value;
                            }
                        }
                        // Try and turn the title red/green/black on the front page.
                        tdiv.bgColor='white';
                        if( link_action[current_id] == 'add' ) { tdiv.bgColor='#F0F8FF'; }
                        if( link_action[current_id] == 'delete' ) { tdiv.bgColor='#FFB6C1'; }

                     }
                     else {
                        $update_ie

                        // Save the email template
                        var idx = document.input_form.email_template.selectedIndex;
                        email_template[current_id] = document.input_form.email_template.options[idx].value;

                        // Save the qualifier
                        var qidx = document.input_form.qualifier.selectedIndex;
                        if ( qidx >= 0 ) { qualifier[current_id] = document.input_form.qualifier.options[qidx].value; }

                        // Save the desired action
                        for (var i=0; i<3; i++) {
                            if (document.input_form.link_action[i].checked=='1') {
                                link_action[current_id] = document.input_form.link_action[i].value;
                            }
                        }
                        // Try and turn the title red/green/black on the front page.
                        tdiv.style.color='black';
                        if( link_action[current_id] == 'add' ) { tdiv.style.background='#F0F8FF'; }
                        if( link_action[current_id] == 'delete' ) { tdiv.style.background='#FFB6C1'; }
                     }
                     closeForm();
                  }

                  function view_desc_image() {
                      if ( document.input_form.desc_image.value != '' ) {
                         var url = document.input_form.desc_image.value;
                         X=window.open(url,'X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=400,height=350');
                      }
                      else { alert('No Description Image Specified'); }
                  }

                  function view_site_icon() {
                      if ( document.input_form.site_icon.value != '' ) {
                         var url = document.input_form.site_icon.value;
                         X=window.open(url,'X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=400,height=350');
                      }
                      else { alert('No Site Icon Specified'); }                  }



                  function changeCategory() {

                      var cid = document.input_form.category.value;
                      X=window.open('$SCRIPT_NAME?admin=hyperseek&action=category_option_form&c='+cid,'X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=400,height=350');
                  }

                  function visitLink() { X=window.open(url[current_id],'X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=630,height=400'); }

                  function emailLink() { self.location.href='mailto:' + email[current_id]; }

                  function loadBigData(form) {
                       var str = '';
                       // Populate the bigdata field with values we can use.
                       for (var i = 0; i < ids.length; ++i) {
                           var val = '';

                           val = val + id[ids[i]] + '===';
                           val = val + category[ids[i]] + '===';
                           val = val + extra_link[ids[i]] + '===';
                           val = val + rank[ids[i]] + '===';
                           val = val + qualifier[ids[i]] + '===';
                           val = val + email[ids[i]] + '===';
                           val = val + title[ids[i]] + '===';
                           val = val + url[ids[i]] + '===';
                           val = val + description[ids[i]] + '===';
                           val = val + keywords[ids[i]] + '===';
                           val = val + desc_image[ids[i]] + '===';
                           val = val + title_tag[ids[i]] + '===';
                           val = val + site_icon[ids[i]] + '===';
                           val = val + name[ids[i]] + '===';
                           val = val + address[ids[i]] + '===';
                           val = val + city[ids[i]] + '===';
                           val = val + state[ids[i]] + '===';
                           val = val + zip[ids[i]] + '===';
                           val = val + country[ids[i]] + '===';
                           val = val + phone[ids[i]] + '===';
                           val = val + fax[ids[i]] + '===';
                           val = val + startdate[ids[i]] + '===';
                           val = val + enddate[ids[i]] + '===';
                           val = val + price[ids[i]] + '===';
                           val = val + account[ids[i]] + '===';
                           val = val + xfactor[ids[i]] + '===';
                           val = val + active[ids[i]] + '===';
                           val = val + max_limit[ids[i]] + '===';
                           val = val + max_period[ids[i]] + '===';
                           val = val + field1[ids[i]] + '===';
                           val = val + field2[ids[i]] + '===';
                           val = val + field3[ids[i]] + '===';
                           val = val + field4[ids[i]] + '===';
                           val = val + field5[ids[i]] + '===';
                           val = val + field6[ids[i]] + '===';
                           val = val + field7[ids[i]] + '===';
                           val = val + field8[ids[i]] + '===';
                           val = val + field9[ids[i]] + '===';
                           val = val + field10[ids[i]] + '===';

                           val = val + submitted_by[ids[i]] + '===';
                           val = val + submission_type[ids[i]] + '===';

                           val = val + email_template[ids[i]] + '===';
                           val = val + link_action[ids[i]];

                           str = str + val + '$split';

                       }
                       document.submission_form.bigdata.value=str;
                  }

                  //Calls the init function on pageload.
                  onload=changeTextInit

              </SCRIPT>

              <form name='submission_form' action='$SCRIPT_NAME' method='post' onsubmit='loadBigData();'>
              <table align='left' border=0 cellspacing=0 cellpadding=4>
                 <tr>
                    <td valign='top' align='left' colspan=3>
                       <p style='color:navy; font-weight:bold; font-size:14pt;'>$language[queue_header] $CGI[type] <input name='holder' style='border:0px; width:1px; background:white' value=''></p>
                       <b>$nav</b>
                    </td>
                 </tr>

                 $list

                 <tr>
                   <td colspan=3 align='center'>
                     <input type='hidden' name='admin' value='hyperseek'>
                     <input type='hidden' name='action' value='process_submissions'>
                     <input type='hidden' name='bigdata' value=''>
                     <input type='submit' name='pbutton' value='$language[queue_button]'>
                   </td>
                 </tr>

              </table>
              </form>

              <b>$nav</b>

              <div id='divMessage'>
                &nbsp;
              </div>

              <table height=1000><tr><td>&nbsp;</td></tr></table>

           ";
						admin_output ( "$language[queue_page_title] $CGI[type]" , $QUEUE );
				}

				#--------------------------------------------------------------------------#
				#  Define search/listing qualifiers                                        #
				#--------------------------------------------------------------------------#
				function category_qualifiers ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $config;
						global $language;
						global $data_dir;
						$message = '';
						$qlist   = '';
						$qfile   = "$data_dir/hyperseek/qualifiers.dat";
						if ( isset( $CGI[ qualifiers ] ) ) {
								if ( empty( $CGI[ qualifiers ] ) ) {
										if ( unlink ( $qfile ) ) {
												$message = "<H2>Qualifiers Deleted</H2>";
										} else {
												$message = "<H2 style='color:maroon'>Could not delete $qfile.  Please delete manually</H2>";
										}
								} else {
										write_file ( $qfile , $CGI[ qualifiers ] );
										$message = "<H2>Qualifiers Saved</H2> <pre>$CGI[qualifiers]</pre>";
								}
						} else {
								if ( file_exists ( $qfile ) ) {
										$qlist = implode ( "" , file ( $qfile ) );
								}
								$message = "
                 <table width='400'>
                     <tr>
                       <td>
                         <form action='$SCRIPT_NAME' mehtod='post'>
                            $language[category_qualifiers_description]<br /><br />

                            <textarea cols=60 rows=10 name='qualifiers'>$qlist</textarea><br />

                            <input type='hidden' name='action' value='category_qualifiers'>
                            <input type='hidden' name='do' value='save'>
                            <input type='hidden' name='admin' value='hyperseek'>

                            <center><input type='submit' value='$language[category_qualifiers_button]'></center>
                         </form>
                       </td>
                     </tr>
                   </table>
               ";
						}
						admin_output ( $language[ category_qualifiers_title ] , $message );
				}

				function process_submissions ( $via_email = '' )
				{
						global $iSQL;
						global $CGI;
						global $config;
						global $language;
						$output       = '';
						$split        = "----------";
						$generated_id = time ();
						## List of possible emails to send.
						$SQL     = "SELECT * FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'EMAIL'";
						$letters = $iSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $letters as $email ) {
								$templates[ $email[ template_name ] ] = $email[ template_text ];
						}
						## Master fieldname Array
						$fieldnames = array ( 'id' , 'category' , 'extra_link' , 'rank' , 'qualifier' , 'email' , 'title' , 'url' , 'description' , 'keywords' , 'desc_image' , 'title_tag' , 'site_icon' , 'name' , 'address' , 'city' , 'state' , 'zip' , 'country' , 'phone' , 'fax' , 'startdate' , 'enddate' , 'price' , 'account' , 'xfactor' , 'active' , 'max_limit' , 'max_period' , 'field1' , 'field2' , 'field3' , 'field4' , 'field5' , 'field6' , 'field7' , 'field8' , 'field9' , 'field10' , 'submitted_by' , 'submission_type' , 'email_template' , 'action' );
						$links      = split ( $split , $CGI[ bigdata ] );
						foreach ( $links as $link ) {
								$generated_id++;
								$ld       = split ( "===" , $link );
								$linkdata = array ();
								$linkdata = dsx_array_combine ( $fieldnames , $ld );
								if ( !$linkdata[ action ] || $linkdata[ action ] == "undefined" ) {
										continue;
								}
								$temprecord = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $linkdata[ id ] ) );
								$OK         = 1;
								if ( $moderator && $moderator_categories[ $linkdata[ category ] ][ can_approve_listings ] != "Y" ) {
										$OK = 0;
								}
								## Main admin, or a moderator with permissions ...
								if ( !$linkdata[ id ] ) {
										continue;
								}
								if ( $OK ) {
										$output .= "
                        $language[queue_processing] <b>$linkdata[action]</b> ...
                        $linkdata[submission_type] <br />
                        $linkdata[title] <br />
                        $linkdata[url] <br />
                        $language[queue_sending_email]: $linkdata[email_template]
                        <br /><br />
                     ";
										if ( $linkdata[ action ] == "add" ) {
												## For new, spidered, assign a new ID, so what's new works right.
												if ( preg_match ( "/new|spidered/i" , $linkdata[ submission_type ] ) ) {
														$linkdata[ id ] = $generated_id;
												}
												// CUSTOM: Gamboozle
												if ( $linkdata[ 'field1' ] == "Yes" ) {
														$CGI[ 'zone' ] = "SKY";
												}
												// Rename the image from it's tmp name, and re-seave the listing with the new image name
												if ( preg_match ( "/\-tmp/i" , $linkdata[ 'desc_image' ] ) ) {
														$filename                 = basename ( $linkdata[ 'desc_image' ] );
														$parts                    = explode ( "." , $filename );
														$ext                      = $parts[ 1 ];
														$orig_image_path          = $config[ 'upload_directory_path' ] . "/" . $filename;
														$new_image_path           = $config[ 'upload_directory_path' ] . "/" . $linkdata[ id ] . "." . $ext;
														$linkdata[ 'desc_image' ] = $config[ 'upload_directory_url' ] . "/" . $linkdata[ id ] . "." . $ext;
														rename ( $orig_image_path , $new_image_path );
														echo "rename($orig_image_path, $new_image_path)<BR>";
												}
												// End CUSTOM Gamboozle
												$permanentrecord = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $linkdata[ id ] ) );
												list ( $saved , $savemsg ) = $permanentrecord->save ( $linkdata );
												if ( $saved ) {
														$output .= $language[ queue_deleting_temp ] . "<br />";
														$output .= $temprecord->delete () . "<br />";
														$output .= $savemsg . "<br />";
														$index = $permanentrecord->reindex ( $stoplist );
														$cats  = array ();
														foreach ( split ( ";" , $linkdata[ category ] ) as $cat ) {
																array_push ( $cats , $cat );
														}
														$categorize = $permanentrecord->categorize ( $cats );
														$output .= $permanentrecord->dump_data () . $index . $categorize;
														if ( $linkdata[ email_template ] ) {
																foreach ( $linkdata as $key => $val ) {
																		$newkey              = strtoupper ( $key );
																		$linkdata[ $newkey ] = $val;
																}
																$SQL                       = "SELECT full_name FROM " . CATEGORY_TABLE . " WHERE category_id = '$linkdata[category]'";
																$linkdata[ category_name ] = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
																$template                  = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $templates[ $linkdata[ email_template ] ] , "values" => $linkdata ) );
																$body                      = $template->merge ();
																mail ( $linkdata[ email ] , $linkdata[ email_template ] , $body , "From: $config[email]\r\n" );
														}
												} else {
														$output .= "<span style='color:red'>$savemsg</span><br />";
												}
												$output .= "<hr />\n";
										} else {
												$output .= $language[ queue_deleting_temp ] . " ";
												$output .= $temprecord->delete ();
												// CUSTOM: Gamboozle
												if ( $linkdata[ 'field1' ] == "Yes" ) {
														$CGI[ 'zone' ] = "SKY";
												}
												// Rename the image from it's tmp name, and re-seave the listing with the new image name
												if ( preg_match ( "/\-tmp/i" , $linkdata[ 'desc_image' ] ) ) {
														$filename        = basename ( $linkdata[ 'desc_image' ] );
														$orig_image_path = $config[ 'upload_directory_path' ] . "/" . $filename;
														unlink ( $orig_image_path );
												}
												// End CUSTOM Gamboozle
												if ( $linkdata[ email_template ] ) {
														foreach ( $linkdata as $key => $val ) {
																$newkey              = strtoupper ( $key );
																$linkdata[ $newkey ] = $val;
														}
														$SQL                       = "SELECT full_name FROM " . CATEGORY_TABLE . " WHERE category_id = '$linkdata[category]'";
														$linkdata[ category_name ] = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
														$template                  = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $templates[ $linkdata[ email_template ] ] , "values" => $linkdata ) );
														$body                      = $template->merge ();
														mail ( $linkdata[ email ] , $linkdata[ email_template ] , $body , "From: $config[email]\r\n" );
												}
												$output .= "<br />";
										}
										## If this approval is via another moderater (ie: approving the approval), count and save it.
										if ( $moderator ) {
												$real_id = $moderator->data[ acct_id ];
												$mod_id  = $linkdata[ submitted_by ];
												if ( !$mod_id ) {
														$mod_id = $real_id;
												}
												## Mark the addition for the moderator.
												if ( preg_match ( "/new|spidered|mod_approved/i" , $linkdata[ submission_type ] ) && $linkdata[ action ] == "add" ) {
														$output .= $this->log_moderator_activity ( array ( "moderator_id" => $mod_id , "category_id" => $linkdata[ category ] , "action" => 'add' ) );
												}
												## If this moderator approved or rejected someone elses suggestion for add or edit.....
												if ( $mod_id != $real_id ) {
														if ( $linkdata[ action ] == "add" ) {
																$output .= $this->log_moderator_activity ( array ( "moderator_id" => $mod_id , "category_id" => $linkdata[ category ] , "action" => 'approve' ) );
														} else {
																$output .= $this->log_moderator_activity ( array ( "moderator_id" => $mod_id , "category_id" => $linkdata[ category ] , "action" => 'reject' ) );
														}
												}
										}
										##
								} ## Moderator without permissions ...
								else {
										$output .= "$language[moderator_cant_approve]<br />";
										if ( $linkdata[ action ] == "add" ) {
												$temprecord->data[ submission_type ] = "MOD_APPROVED";
										} else {
												$temprecord->data[ submission_type ] = "MOD_REJECT";
										}
										$temprecord->data[ submitted_by ] = $moderator->data[ acct_id ];
										$temprecord->update_values ( $temprecord->data );
										$temprecord->save ();
								}
						}
						if ( $via_email ) {
								return $output;
						} else {
								admin_output ( $language[ queue_process_title ] , $output );
						}
				}

				function approve_via_email ()
				{
						global $CGI;
						$CGI[ bigdata ] = $CGI[ id ] . "===";
						$CGI[ bigdata ] .= $CGI[ category ] . "===";
						$CGI[ bigdata ] .= $CGI[ extra_link ] . "===";
						$CGI[ bigdata ] .= $CGI[ rank ] . "===";
						$CGI[ bigdata ] .= $CGI[ qualifier ] . "===";
						$CGI[ bigdata ] .= $CGI[ email ] . "===";
						$CGI[ bigdata ] .= $CGI[ title ] . "===";
						$CGI[ bigdata ] .= $CGI[ url ] . "===";
						$CGI[ bigdata ] .= $CGI[ description ] . "===";
						$CGI[ bigdata ] .= $CGI[ keywords ] . "===";
						$CGI[ bigdata ] .= $CGI[ desc_image ] . "===";
						$CGI[ bigdata ] .= $CGI[ title_tag ] . "===";
						$CGI[ bigdata ] .= $CGI[ site_icon ] . "===";
						$CGI[ bigdata ] .= $CGI[ name ] . "===";
						$CGI[ bigdata ] .= $CGI[ address ] . "===";
						$CGI[ bigdata ] .= $CGI[ city ] . "===";
						$CGI[ bigdata ] .= $CGI[ state ] . "===";
						$CGI[ bigdata ] .= $CGI[ zip ] . "===";
						$CGI[ bigdata ] .= $CGI[ country ] . "===";
						$CGI[ bigdata ] .= $CGI[ phone ] . "===";
						$CGI[ bigdata ] .= $CGI[ fax ] . "===";
						$CGI[ bigdata ] .= $CGI[ startdate ] . "===";
						$CGI[ bigdata ] .= $CGI[ enddate ] . "===";
						$CGI[ bigdata ] .= $CGI[ price ] . "===";
						$CGI[ bigdata ] .= $CGI[ account ] . "===";
						$CGI[ bigdata ] .= $CGI[ xfactor ] . "===";
						$CGI[ bigdata ] .= $CGI[ field1 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field2 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field3 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field4 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field5 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field6 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field7 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field8 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field9 ] . "===";
						$CGI[ bigdata ] .= $CGI[ field10 ] . "===";
						$CGI[ bigdata ] .= $CGI[ submitted_by ] . "===";
						$CGI[ bigdata ] .= $CGI[ submission_type ] . "===";
						$CGI[ bigdata ] .= $CGI[ email_template ] . "===";
						$todo = "add";
						if ( $CGI[ reject ] ) {
								$todo = 'delete';
						}
						$CGI[ bigdata ] .= $todo;
						$queue_out = $this->process_submissions ( "email" );
						$input     = '';
						foreach ( $CGI as $key => $value ) {
								$input .= "$key: $value<br /><br />";
						}
						echo $queue_out;
						exit;
				}


				#--------------------------------------------------------------------------#
				#  Listing Browser / Utilities                                             #
				#--------------------------------------------------------------------------#
				function category_option_form ()
				{
						global $language;
						global $CGI;
						$cid     = $CGI[ 'c' ];
						$options = category_option_list ( $cid );
						echo "
              <head>
                <title>Category Selector</title>
                <script language='javascript'>

                 var IE = (document.all) ? 1 : 0;

                 function changeCategory() {

                    var selected_category = '';
                    var newcategory = '';
                    var found = 0;

                    if ( IE )

                    for (var i = 0; i < document.catform.category.options.length; i++) {
                       if (document.catform.category.options[i].selected == 1 ) {
                          if( found == 1) {
                             selected_category = selected_category + ';';
                             newcategory = newcategory + ',<br />';
                          }
                          found=1;
                          selected_category = selected_category + document.catform.category.options[i].value;
                          newcategory = newcategory + document.catform.category.options[i].text;
                       }
                    }

                    if( IE ) {
                         opener.document.input_form.category.value = selected_category;
                         tdiv = opener.document.all['cat'];
                         if (tdiv) {
                            tdiv.innerHTML = newcategory;
                            tdiv.style.color='blue';
                         }
                         cnamediv = opener.document.all['catname'];
                         if (cnamediv) {
                            cnamediv.innerHTML = newcategory;
                         }
                    }
                    else {
                         tdiv = opener.document.getElementById('cat');
                         cnamediv = opener.document.getElementById('catname');
                         if ( opener.document.divMessage ) {
                             opener.document.divMessage.document.input_form.category.value = selected_category;
                         }
                         else {
                             opener.document.input_form.category.value = selected_category;
                             if (tdiv) {
                                 tdiv.innerHTML = newcategory;
                                 tdiv.bgColor='#e0e0e0';
                             }
                             if (cnamediv) {
                                 cnamediv.innerHTML = newcategory;
                             }
                         }
                    }

                    self.close();
                 }

                </script>
              </head>
              <body bgcolor='#e0e0e0'>
              <form name='catform'>
              <table border=0 width=85% align='center'>
                 <tr>
                   <td align='center'>
                       <b>$language[select_category_prompt]</b><br />
                       <select name='category' multiple size=10>
                       $options
                       </select><br /><br />
                       <input type='button' onclick='changeCategory($msie)' value='$language[select_category_button]'>
                   </td>
                 </tr>
              </table>
              </form>
              </body>
           ";
						exit;
				}


				#--------------------------------------------------------------------------#
				#  Category Browser right side content                                     #
				#--------------------------------------------------------------------------#
				function directory_content ()
				{
						global $CGI;
						global $oSQL;
						if ( $CGI[ 'Terms' ] || $CGI[ 'search' ] == "NEW" || $CGI[ 'search' ] == "COOL" ) {
								return $this->search_results ();
						}
						if ( !$CGI[ category ] ) {
								$CGI[ category ] = $CGI[ category_id ];
						}
						if ( $moderator ) {
								$OK = 0;
						} else {
								$OK = 1;
						}
						if ( $moderator && ( $CGI[ category ] && $moderator_categories[ $CGI[ category ] ] && $moderator_categories[ $CGI[ category_id ] ] != "NOT" ) || ( !$CGI[ category_id ] )
						) {
								$OK = 1;
						}
						$category = new Record ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						$name     = $category->data[ full_name ];
						## Get Links ...
						if ( $OK && $CGI[ 'category' ] ) {
								$linkstable = LINKS_TABLE;
								$catstable  = CATS_TABLE;
								$SQL        = "SELECT count(*) FROM $linkstable, $catstable WHERE $catstable.category = $CGI[category] AND $catstable.id = $linkstable.id ORDER BY $linkstable.title";
								$count      = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
								if ( $CGI[ 'page' ] ) {
										$start = ( $CGI[ 'page' ] - 1 ) * 25;
								} else {
										$start = 0;
								}
								$SQL     = "
                  SELECT $linkstable.*
                      FROM $linkstable, $catstable
                      WHERE $catstable.category = $CGI[category]
                      AND $catstable.id = $linkstable.id
                      ORDER BY $linkstable.title
                      LIMIT $start, 25
               ";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								list ( $linklist , $navbar ) = $this->link_list ( $results , $count );
						} else {
								$navbar = "This is not your category";
						}

						return $this->show_links ( $name , $linklist , $navbar );
				}

				function search ()
				{
						directory ();
						exit;
				}

				function search_results ()
				{
						global $CGI;
						global $oSQL;
						global $config;
						$links_table = LINKS_TABLE;
						$keys_table  = KEYS_TABLE;
						## Overrides ##
						$config[ jackhammer_no_price ] = "NO";
						$config[ yh_new_display ]      = "SHOW ALL";
						list ( $search_by , $value ) = split ( ":" , $CGI[ Terms ] , 2 );
						if ( $CGI[ prefix ] ) {
								$search_by = $CGI[ prefix ];
								$value     = $CGI[ Terms ];
						}
						if ( $search_by == "mailto" ) {
								$search_by = "email";
						}
						if ( $search_by == "url" || $search_by == "id" || $search_by == "email" ) {
								$header       = "Match: $search_by: $value";
								$CGI[ Terms ] = $search_by . ":" . $value;
								$SQL          = "
                  SELECT *
                  FROM  $links_table
                  WHERE $search_by LIKE '%$value%'
              ";
						} elseif ( preg_match ( "/cool/i" , $CGI[ search ] ) ) {
								$header = "Cool Listings";
								$where  = "rank >= $config[yh_cool]";
								$SQL    = "
                  SELECT *
                  FROM $links_table
                  WHERE $where
                  LIMIT $config[yh_max_results]
              ";
						} elseif ( preg_match ( "/new/i" , $CGI[ search ] ) ) {
								$header = "New Listings";
								$now    = time ();
								$days   = $config[ 'yh_new_days' ];
								$low    = $now - ( 86400 * $days );
								$high   = $now;
								$SQL    = "
                  SELECT *
                  FROM $links_table
                  WHERE id >= $low AND id <= $high
                  ORDER BY id DESC
                  LIMIT $config[yh_max_results]
              ";
						} else {
								$header      = $CGI[ 'Terms' ];
								$lookupterms = $CGI[ 'Terms' ];
								if ( preg_match ( "/\s+/" , $lookupterms ) && !preg_match ( "/\"|\'/" , $lookupterms ) ) {
										$lookupterms = $lookupterms . " or (\"$lookupterms\")";
								}
								$lookupterms = preg_replace ( "/\'/" , "\"" , $lookupterms );
								$match       = "title,description,keywords";
								if ( $config[ fti_fields ] ) {
										$match = $config[ fti_fields ];
								}
								$boolean         = '';
								$query_expansion = '';
								if ( !preg_match ( "/\s+/" , $lookupterms ) && $config[ sql_use_expansion ] == "YES" ) {
										$query_expansion = 'WITH QUERY EXPANSION';
								} elseif ( $config[ sql_use_boolean ] == "YES" ) {
										$boolean = 'IN BOOLEAN MODE';
								}
								if ( preg_match ( "/bids/i" , $CGI[ prefix ] ) ) {
										$SQL = "
                        SELECT $links_table.*, $keys_table.*
                        FROM $links_table, $keys_table
                        WHERE $keys_table.keyword = '$lookupterms'
                        AND $keys_table.id = $links_table.id
                        ORDER BY bid_price DESC, last_modified ASC
                   ";
								} else {
										$SQL = "
                        SELECT $links_table.*,
                               MATCH($match) AGAINST ( '$lookupterms' $boolean $query_expansion ) AS score
                        FROM $links_table
                        WHERE MATCH($match) AGAINST ( '$lookupterms' $boolean $query_expansion )
                        ORDER BY score DESC
                   ";
								}
						}
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						$matches = count ( $results );
						if ( $matches ) {
								list( $linklist , $navbar ) = $this->link_list ( $results );
						}
						list ( $content , $menu ) = $this->show_links ( "$language[search_results] <i>$header</i> ... $matches $language[search_results_found]" , $linklist , $navbar );

						return array ( $content , '' );
						exit;
				}

				#--------------------------------------------------------------------------#
				#  Get a list of site matches, for category and searches                   #
				#--------------------------------------------------------------------------#
				function link_list ( $links = array () , $cnt = 0 )
				{
						global $CGI;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						if ( $cnt ) {
								$num_results = $cnt;
						} else {
								$num_results = count ( $links );
						}
						$num_per_page = 25;
						if ( $num_results > $num_per_page ) {
								$num_pages = floor ( $num_results / $num_per_page );
								if ( $num_results % $num_per_page ) {
										$num_pages++;
								}
						} else {
								$num_pages = 1;
						}
						$nblinks = array ();
						if ( !$CGI[ page ] ) {
								$CGI[ page ] = 1;
						}
						for ( $x = 1; $x <= $num_pages; $x++ ) {
								if ( $CGI[ page ] == $x ) {
										array_push ( $nblinks , "<b>$x</b>" );
								} else {
										$eterms = urlencode ( $CGI[ Terms ] );
										$link   = "$SCRIPT_NAME?admin=hyperseek&action=$CGI[action]&full_tree=$CGI[full_tree]&Terms=$eterms&prefix=$CGI[prefix]&category_id=$CGI[category_id]&search=$CGI[search]&page=$x";
										array_push ( $nblinks , "<a href='$link'>$x</a>" );
								}
						}
						if ( $cnt ) {
								$listings = $links;
						} else {
								$listings = array ();
								$start    = ( $CGI[ page ] - 1 ) * 25;
								$end      = $start + $num_per_page;
								if ( $end >= $num_results ) {
										$end = $num_results;
								}
								if ( $num_pages > 1 ) {
										$showlinks = $links;
										$listings  = array_slice ( $showlinks , $start , $end );
								} else {
										$listings = $links;
								}
						}
						$start++;
						// if ( $end < $num_results ) { $end++; }  # For display purposes only
						$nav_bar = "Page: " . implode ( " - " , $nblinks ) . "<br /> Showing: $start - $end";
						$terms   = urlencode ( $CGI[ Terms ] );
						foreach ( $listings as $link ) {
								if ( $link[ 'bid_price' ] ) {
										$paid = 1;
								}
								$catlink  = " <a href='$SCRIPT_NAME?action=directory&category_id=$link[category]'>$link[category]</a> ";
								$add_date = formatdate ( datestamp ( $link[ id ] ) );
								$bidrow   = '';
								if ( $paid ) {
										$bid_price = $link[ 'bid_price' ];
										if ( $bid_price < 0.00 ) {
												$color = 'red';
										} elseif ( $bid_price == 0.00 ) {
												$color = "black";
										} else {
												$color = 'blue';
										}
										$bidrow = "
                         <td valign='top' align='right' style='color:$color'>
                             $bid_price
                         </td>
                      ";
								};
								$linksns .= "

                      <tr>
                        <td valign='top'>
                           <a href='$link[url]' target='new'>$link[title]</a>
                        </td>
                        <td valign='top'>
                           $link[account]
                        </td>
                        $bidrow
                        <td valign='top' nowrap>
                                <a href='javascript:load_url(\"$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]&category=$CGI[category_id]&Terms=$terms\")'>$language[search_results_edit_link]</a> -
                                <a href='javascript:load_url(\"$SCRIPT_NAME?admin=hyperseek&action=review_link&id=$link[id]&category=$CGI[category_id]&Terms=$terms\")'>$language[search_results_review_link]</a> -
                                <a href='mailto:$link[email]'>$language[search_results_email_link]</a> -
                                <a href='javascript:load_url(\"$SCRIPT_NAME?admin=hyperseek&action=feature_link&id=$link[id]&category=$CGI[category_id]&Terms=$terms\")'>$language[search_results_feature_link]</a> -
                                <a href='javascript:load_url(\"$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$link[id]&category=$CGI[category_id]&Terms=$terms\")'>$language[search_results_delete_link]</a>
                        </td>
                      </tr>

                  ";
						}
						if ( $paid ) {
								$bid_price_header = "<td nowrap>Bid ($language[dollar_sign])</td>";
						}
						$list = "
                <table border='1' cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td nowrap>$language[search_results_title]</td>
                        <td nowrap>$language[search_results_account]</td>
                        $bid_price_header
                        <td nowrap>&nbsp;</td>
                    </tr>
                </thead>
                <tbody>
                   $linksns
                </tbody>
                </table>

              ";

						return array ( $list , $nav_bar );
				}

				#--------------------------------------------------------------------------#
				#  Show actual search result output                                        #
				#--------------------------------------------------------------------------#
				function show_links ( $title = '' , $linklist = '' , $navbar = '' , $tree = '' )
				{
						global $config;
						global $language;
						global $CGI;
						global $SCRIPT_NAME;
						$terms     = urlencode ( $CGI[ Terms ] );
						$encname   = cleanup_string ( $title , "_" , 1 );
						$edit_link = " <td align='center'><a href='$SCRIPT_NAME?action=edit_file&filename=$encname'>Page Template</a></td> ";
						if ( $moderator && $moderator_categories[ $CGI[ category_id ] ][ access_level ] == "Admin" ) {
								$edit_link = "";
						}
						if ( !$CGI[ category_id ] ) {
								$CGI[ category_id ] = 0;
						}
						$category_menu = category_menu ( $title );
						if ( $moderator && $moderator_categories[ $CGI[ category_id ] ][ access_level ] == "Admin" ) {
								$category_menu = "";
						}
						if ( $CGI[ action ] == "search" ) {
								$category_menu = '';
						}
						if ( $CGI[ 'prefix' ] == "bids" ) {
								$b_selected = "selected";
						}
						if ( $CGI[ 'prefix' ] == "id" ) {
								$i_selected = "selected";
						}
						if ( $CGI[ 'prefix' ] == "email" ) {
								$e_selected = "selected";
						}
						if ( $CGI[ 'prefix' ] == "url" ) {
								$u_selected = "selected";
						}
						if ( $CGI[ 'prefix' ] == "template" ) {
								$t_selected = "selected";
						}
						$content = "
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

               <form action='$SCRIPT_NAME' target='main'>
               <center>
                  <select name='prefix'>
                     <option value=''>$language[search_by_keywords] (Relevancy Only)</option>
                     <option value='bids' $b_selected>$language[search_by_keywords] (PPC Only)</option>
                     <option value='id' $i_selected>$language[search_by_id]</option>
                     <option value='email' $e_selected>$language[search_by_email]</option>
                     <option value='url' $u_selected>$language[search_by_url]</option>
                     <option value='template' $t_selected>$language[search_by_template]</option>
                  </select>
                  <input name='Terms' align = 'top' size='16' value='$CGI[Terms]'>
                  <input name='action' type='hidden' value='search'>
                  <input name='admin' type='hidden' value='hyperseek'>
                  <input type='submit' name='submit' value='$language[search_button]'>
               </center>
               </form>

              <table align='left' width='100%' border=0 cellspacing=10>

                 <tr>
                    <td valign='top' align='left'>
                       $tree
                    </td>
                    <td valign='top' align='left'>
                       <b>$title</b><br />
                       $navbar
                               <ilayer height=35px>
                               <layer visibility=show>
                               <span class=iewrap1>
                                 <span class=iewrap2>&nbsp;</span>
                               </span>
                               </layer>
                               </ilayer>
                       $linklist
                    </td>
                 </tr>
              </table>

              <form name='linkform'><input type='hidden' name='linkid' value=''></form>

           ";

						return array ( $content , $category_menu );
				}


				#--------------------------------------------------------------------------#
				#  Edit a listing                                                          #
				#--------------------------------------------------------------------------#
				function edit_link ()
				{
						global $qdat;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $CGI;
						global $currentlink;
						$qdat        = '';
						$link        = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$currentlink = $link;
						$link_title  = $link->data[ title ];
						list( $mycategory , $junk ) = split ( ";" , $link->data[ category ] );
						if ( !$CGI[ id ] ) {
								$link->data[ id ] = time ();
						}
						if ( $CGI[ category_id ] ) {
								$category               = new Record ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
								$link->data[ category ] = $category->data[ category_id ];
						}
						$form      = '';
						$formarray = $link->generate_form ();
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								preg_match ( "/name\=(\"|\')(.*?)(\"|\')/" , $fieldinput , $elements );
								$fieldname = $elements[ 2 ];
								if ( $fieldname == "category" ) {
										$cats = array ();
										foreach ( split ( ";" , $link->data[ category ] ) as $category ) {
												$cat = new Record ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $category ) );
												$qdat .= $cat->data[ qualifiers ];
												array_push ( $cats , $cat->data[ full_name ] );
										}
										$catlist = implode ( "," , $cats );
										$form .= "
                      <tr>
                         <td bgcolor='#e0e0e0' valign='top' align='left' width=25%>
                            <b><a href='javascript:changeCategory()'>$prompt</a></b>
                            $fieldinput
                         </td>
                         <td bgcolor='silver' valign='top' align='left' width=75%>
                            <div id='cat'>
                              $catlist
                            </div>
                         </td>
                      </tr>
                 ";
								} elseif ( $fieldname == "url" ) {
										$url = $link->data[ url ];
										$form .= "
                   <tr>
                      <td bgcolor='#e0e0e0' valign='top' align='left' width='25%'>
                         <b><a href='$url' target='new'>$prompt</a></b>
                      </td>
                      <td bgcolor='silver' valign='top' align='left' width='75%'>
                         $fieldinput
                      </td>
                   </tr>
                ";
								} else {
										$form .= "<tr bgcolor='#e0e0e0'><td>$prompt</td><td bgcolor='silver'>$fieldinput</td></tr>\n";
								}
						}
						$OUTPUT = "
              <script language='javascript'>
                 function changeCategory() {
                     X=window.open('$SCRIPT_NAME?admin=hyperseek&action=category_option_form','X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=400,height=350');
                 }
              </script>
              <style>
                 #cat [position:relative;]
              </style>
              <table align='left' border=0 cellspacing=0 cellpadding=4>
                 <tr>
                    <td valign='top' align='left'>
                       <b>$language[edit_link_title] $link_title</b>
                       <form name='input_form' action='$SCRIPT_NAME' method='post'>
                       <table border=0 cellspacing=0 cellpadding=4>
                          <tr bgcolor='navy'>
                            <td colspan=2>
                               <font color='white'>
                               <b>$language[search_results_link_menu_title]</b>
                                  &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=feature_link&id=$CGI[id]&category=$mycategory'><FONT COLOR='white'>$language[search_results_feature_link]</FONT></a>
                                  &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$CGI[id]'><FONT COLOR='white'>$language[search_results_delete_link]</FONT></a>
                                  &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=review_link&id=$CGI[id]'><FONT COLOR='white'>$language[search_results_review_link]</FONT></a>
                               </font>
                            </td>
                          </tr>
                          $form
                          <tr><td colspan=2 align='center'>
                             <input type='hidden' name='action' value='save_link'>
                             <input type='hidden' name='id' value='$CGI[id]'>
                             <input type='hidden' name='admin' value='hyperseek'>
                             <input type='submit' value='$language[save_link_button]'>
                          </td></tr>
                       </table>
                       </form>
                    </td>
                 </tr>
              </table>

           ";
						admin_output ( "$language[edit_link_title]<i>$CGI[id] / $link_title</i>" , $OUTPUT );
				}

				#-------------------------------------------------------------------------------------------------------#
				# Add a global listing. This function add by Akhilesh on 1/4/2011 for add new RON  campaign from admin  #
				#-------------------------------------------------------------------------------------------------------#
				function add_link ()
				{
						global $qdat;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $CGI;
						global $currentlink;
						// Set the ID //
						if ( empty( $CGI[ 'id' ] ) ) {
								$CGI[ 'id' ] = time ();
						}
						$saveaction = "save_submission";
						// Set Valid Account ? //
						$SQL     = "SELECT acct_id FROM " . ACCOUNTS_TABLE . " WHERE `acct_type` = 'advertiser' or `acct_type` ='' ORDER BY acct_id";
						$results = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'set' ) );
						while ( list ( $ptr , $advertiser ) = each ( $results ) ) {
								$advertiser_list .= "<option value='" . htmlspecialchars ( $advertiser[ 'acct_id' ] ) . "'>" . htmlspecialchars ( $advertiser[ 'acct_id' ] ) . "</option>\n";
						}
						$schema = "hyperseek_temp_addurl";
						if ( $CGI[ 'schema' ] ) {
								$schema = $CGI[ 'schema' ];
						}
						$link = new Record( array ( "Parent" => $oSQL , "Table" => TEMP_TABLE , "Schema" => $schema ) );
						$link->update_values ( $CGI );
						$formvalues = $link->generate_form ();
						$form       = '';
						//Select option for advertiser
						$form .= "<tr><td><b>Select Advertiser</b></td><td><select name='account'><option value=''> Select Advertiser</option>$advertiser_list</select></td></tr>\n";
						while ( list ( $prompt , $fieldinput ) = each ( $formvalues ) ) {
								if ( preg_match ( "/name='category'/i" , $fieldinput ) ) {
										$fieldinput = $catform;
								}
								$form .= "<tr><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						// Select option for GEO Target
						$form .= '<tr><td><b>Current Geo Target:</b></td><td><select name="zone">';
						$zonelist = explode ( "," , $config[ 'bidding_zones' ] );
						foreach ( $zonelist as $item ) {
								list ( $zoneid , $zonename ) = preg_split ( "/\:/" , $item );
								if ( !$zonename ) {
										$zonename = $zoneid;
								}
								$selected = '';
								if ( $zoneid == $zone ) {
										$selected = "selected='selected'";
								}
								$form .= "<option value='$zoneid' $selected>$zonename</option>\n";
						}
						$form .= "</select></td></tr>";
						// Add Bid price
						$form .= "<tr><td><b>Bid Price: </b></td><td><input type='text' name='bid_price' size='5' value='$config[jackhammer_minimum_bid_ron]' onBlur='chkbidprice()'></td></tr>";
						$OUTPUT = "
		    <!-- <script type=\"text/javascript\" language=\"javascript\">
			function chkbidprice() {
                       var n_bid = document.input_form.bid_price.value;
                       if( n_bid > '0.00' && n_bid < $config[jackhammer_minimum_bid] ) {
                           alert( \"$language{minimum_bid_error} ($config[jackhammer_minimum_bid])\" );
                           document.input_form.bid_price.focus();
                           document.input_form.bid_price.select();
                       }
                  }
			</script> -->
		   <table align='left' border=0 cellspacing=0 cellpadding=4>
                 <tr>
                    <td valign='top' align='left'>
                       <b>Create Ad Campaign</b>
                       <form name='input_form' action='$SCRIPT_NAME' method='post'>
                       <table border=0 cellspacing=0 cellpadding=4>
                          $form
                          <tr><td colspan=2 align='center'>
                             <input type='hidden' name='action' value='save_new_link'>
                             <input type='hidden' name='admin' value='hyperseek'>
							 <input type='hidden' name='id' value='$CGI[id]' />
                             <input type='submit' value='$language[save_link_button]'>
                          </td></tr>
                       </table>
                       </form>
                    </td>
                 </tr>
              </table>

           ";
						admin_output ( "Create New Link <i>$CGI[id] / $link_title</i>" , $OUTPUT );
				}
				#----------------------------------------------------------------------------------#
				#  Save a new listing. This function add by Akhilesh on 1/5/2011 for save new list #
				#----------------------------------------------------------------------------------#
				function save_new_link ( $return = 0 )
				{
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $iSQL;
						global $oSQL;
						global $CGI;
						//print_r($CGI);
						$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$output     = '';
						$link_title = $link->data[ title ];
						if ( $link->data[ id ] && $moderator && $moderator_categories[ $link->data[ category ] ][ can_edit_listings ] != "Y" ) {
								$temp                          = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
								$temp->data[ submission_type ] = "MOD_EDITED";
								$temp->data[ submitted_by ]    = $moderator[ acct_id ];
								if ( $temp->update_values ( $CGI ) ) {
										if ( $temp->save () ) {
												$output = $language[ moderator_cant_change ];
										} else {
												$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
										}
								} else {
										$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
								}
						}
						if ( $link->data[ id ] && $moderator && $moderator_categories[ $link->data[ category ] ][ can_add_listings ] != "Y" ) {
								$temp                          = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
								$temp->data[ submission_type ] = "NEW";
								$temp->data[ submitted_by ]    = $moderator[ acct_id ];
								if ( $temp->update_values ( $CGI ) ) {
										if ( $temp->save () ) {
												$output = $language[ moderator_cant_change ];
										} else {
												$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
										}
								} else {
										$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
								}
						} else {
								### Update Modification Dates on Category Records for new listings.
								if ( !$link->data[ id ] ) {
										$now = time ();
										foreach ( split ( ";" , $CGI[ category ] ) as $cat ) {
												$catsql = "UPDATE " . CATEGORY_TABLE . " SET last_listing = $now WHERE category_id = $cat";
												if ( $cat ) {
														$iSQL->run ( $catsql );
												}
										}
								}
								$cats = array ();
								foreach ( split ( ";" , $CGI[ category ] ) as $cat ) {
										array_push ( $cats , $cat );
								}
								$CGI[ 'rank' ]           = 1;
								$CGI[ 'is_global_list' ] = 'Y';
								$CGI[ 'budget_per_day' ] = $CGI[ 'max_limit' ];
								if ( $CGI[ 'startdate' ] == '' ) {
										$CGI[ 'startdate' ] = date ( 'Ymd' );
								}
								if ( $CGI[ 'zone' ] == '' ) {
										$CGI[ 'country_all' ] = 'ALL';
								} else {
										$CGI[ 'country_all' ] = $CGI[ 'zone' ];
								}
								list( $success , $save ) = $link->save ( $CGI );
								$index                    = $link->reindex ( $stoplist );
								$categorize               = $link->categorize ( $cats );
								$dump                     = $link->dump_data ();
								$xxxx                     = $link->show_debug_info ();
								$groupinfo                = array ();
								$group                    = new Record( array ( "Parent" => $iSQL , "Table" => GROUPS_TABLE , "Schema" => GROUPS_TABLE , "Lookup" => 1 , "camp_id" => $CGI[ id ] ) );
								$groupinfo[ 'camp_id' ]   = $CGI[ 'id' ];
								$groupinfo[ 'name' ]      = $CGI[ 'name' ];
								$groupinfo[ 'stat' ]      = 1;
								$groupinfo[ 'bid_price' ] = $CGI[ 'price' ];
								$group->update_values ( $groupinfo );
								$group->save ();
								if ( $group->update_values ( $groupinfo ) ) {
										$linksSQL = "SELECT id FROM " . GROUPS_TABLE . " WHERE camp_id = '$CGI[id]'  ";
										$links    = $oSQL->search ( array ( "SQL" => $linksSQL , "Return" => 'row' ) );
										$group_id = $links[ 0 ][ 'id' ];
								} else {
										$group_id = $iSQL->insert_id ();
								}
								$adsinfos               = array ();
								$ads                    = new Record( array ( "Parent" => $iSQL , "Table" => ADS_TABLE , "Schema" => ADS_TABLE , "Lookup" => 1 , "camp_id" => $CGI[ id ] ) );
								$adsinfos[ 'camp_id' ]  = $CGI[ 'id' ];
								$adsinfos[ 'group_id' ] = $group_id;
								$adsinfos[ 'headline' ] = $CGI[ 'title' ];
								$adsinfos[ 'desc1' ]    = $CGI[ 'description' ];
								$adsinfos[ 'dis_url' ]  = $CGI[ 'url' ];
								$adsinfos[ 'dest_url' ] = $CGI[ 'url' ];
								$adsinfos[ 'status' ]   = 1;
								$ads->update_values ( $adsinfos );
								$ads->save ();
								//Save bid Price
								if ( $success ) {
										$date                       = date ( "YmdHi" );
										$keywordinfo                = array ();
										$keywordinfo[ 'id' ]        = $CGI[ 'id' ];
										$keywordinfo[ 'bid_price' ] = $CGI[ 'price' ];
										$keywordinfo[ 'key_stat' ]  = 1;
										$keywordinfo[ 'group_id' ]  = $group_id;
										//$keywordinfo['bid_price'] = $config['jackhammer_minimum_bid_ron'];
										$keywordinfo[ 'last_modified' ]    = $date;
										$keywordinfo[ 'last_modify_type' ] = "U";
										$keywordinfo[ 'create_date' ]      = $date;
										$keywordinfo[ 'zone' ]             = $CGI[ 'zone' ];
										$keyword                           = new Listing( array ( "Parent" => $iSQL , "Table" => KEYS_TABLE , "Schema" => KEYS_TABLE ) );
										$keyword->update_values ( $keywordinfo );
										$keyword->save ();
								}
								$output = "<b>$save</b>";
						}
						if ( $return ) {
								return $save;
						}
						$OUT = "
              <table align='left' border=0 cellspacing=10 width=98%>
                 <tr>
                    <td valign='top' align='left'>
                     $output
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[link_save_title] <i>$CGI[id] / $link_title</i>" , $OUT );
				}
				#--------------------------------------------------------------------------#
				#  A list of all the currently defined "featured" sites                    #
				#--------------------------------------------------------------------------#
				function list_globaled ()
				{
						global $oSQL;
						global $config;
						global $language;
						global $CGI;
						global $SCRIPT_NAME;
						$report  = '';
						$SQL     = "SELECT * FROM " . LINKS_TABLE . " WHERE is_global_list = 'Y'";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $results as $area ) {
								$report .= "
                 <tr>
                   <td>$area[title]</td>
                   <td>$area[account]</td>
                   <td>
                      [
                      <a href='$area[url]' TARGET='new'>$language[featured_visit]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$area[id]'>$language[featured_edit]</a> |
					  <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$area[id]'>Delete</a>
                      ]
                   </td>
                 </tr>
              ";
						}
						$OUTPUT = "
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

                <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td style='width: 160px;'>Listing Title</td>
						<td style='width: 160px;'>Account</td>
                        <td style='width: 200px;'>$language[featured_action]</td>
                    </tr>
                </thead>
                <tbody>
                   $report
                </tbody>
                </table>
           ";
						admin_output ( 'Global Listings' , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Save a listing                                                          #
				#--------------------------------------------------------------------------#
				function save_link ( $return = 0 )
				{
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $iSQL;
						global $oSQL;
						global $CGI;
						$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$output     = '';
						$link_title = $link->data[ title ];
						if ( $link->data[ id ] && $moderator && $moderator_categories[ $link->data[ category ] ][ can_edit_listings ] != "Y" ) {
								$temp                          = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
								$temp->data[ submission_type ] = "MOD_EDITED";
								$temp->data[ submitted_by ]    = $moderator[ acct_id ];
								if ( $temp->update_values ( $CGI ) ) {
										if ( $temp->save () ) {
												$output = $language[ moderator_cant_change ];
										} else {
												$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
										}
								} else {
										$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
								}
						}
						$groupinfo                = array ();
						$group                    = new Record( array ( "Parent" => $iSQL , "Table" => GROUPS_TABLE , "Schema" => GROUPS_TABLE , "Lookup" => 1 , "camp_id" => $CGI[ id ] ) );
						$groupinfo[ 'camp_id' ]   = $CGI[ 'id' ];
						$groupinfo[ 'name' ]      = $CGI[ 'name' ];
						$groupinfo[ 'stat' ]      = 1;
						$groupinfo[ 'bid_price' ] = $CGI[ 'price' ];
						$group->update_values ( $groupinfo );
						$group->save ();
						if ( $group->update_values ( $groupinfo ) ) {
								$linksSQL = "SELECT id FROM " . GROUPS_TABLE . " WHERE camp_id = '$CGI[id]'  ";
								$camps    = $oSQL->search ( array ( "SQL" => $linksSQL , "Return" => 'row' ) );
								$group_id = $camps[ 0 ][ 'id' ];
						} else {
								$group_id = $iSQL->insert_id ();
						}
						$adsinfo               = array ();
						$ads                   = new Record( array ( "Parent" => $iSQL , "Table" => ADS_TABLE , "Schema" => ADS_TABLE , "Lookup" => 1 , "camp_id" => $CGI[ id ] ) );
						$adsinfo[ 'camp_id' ]  = $CGI[ 'id' ];
						$adsinfo[ 'group_id' ] = $group_id;
						$adsinfo[ 'headline' ] = $CGI[ 'title' ];
						$adsinfo[ 'desc1' ]    = $CGI[ 'description' ];
						$adsinfo[ 'dis_url' ]  = $CGI[ 'url' ];
						$adsinfo[ 'dest_url' ] = $CGI[ 'url' ];
						$adsinfo[ 'status' ]   = 1;
						$ads->update_values ( $adsinfo );
						$ads->save ();
						$keywordinfo                = array ();
						$keyword                    = new Record( array ( "Parent" => $iSQL , "Table" => KEYS_TABLE , "Schema" => KEYS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$keywordinfo[ 'bid_price' ] = $CGI[ 'price' ];
						$keywordinfo[ 'key_stat' ]  = 1;
						$keywordinfo[ 'group_id' ]  = $group_id;
						$keyword->update_values ( $keywordinfo );
						$keyword->save ();
						if ( $link->data[ id ] && $moderator && $moderator_categories[ $link->data[ category ] ][ can_add_listings ] != "Y" ) {
								$temp                          = new Record( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
								$temp->data[ submission_type ] = "NEW";
								$temp->data[ submitted_by ]    = $moderator[ acct_id ];
								if ( $temp->update_values ( $CGI ) ) {
										if ( $temp->save () ) {
												$output = $language[ moderator_cant_change ];
										} else {
												$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
										}
								} else {
										$output = "$language[link_save_error] <pre>$temp->error_text</pre>";
								}
						} else {
								### Update Modification Dates on Category Records for new listings.
								if ( !$link->data[ id ] ) {
										$now = time ();
										foreach ( split ( ";" , $CGI[ category ] ) as $cat ) {
												$catsql = "UPDATE " . CATEGORY_TABLE . " SET last_listing = $now WHERE category_id = $cat";
												if ( $cat ) {
														$iSQL->run ( $catsql );
												}
										}
								}
								$cats = array ();
								foreach ( split ( ";" , $CGI[ category ] ) as $cat ) {
										array_push ( $cats , $cat );
								}
								list( $success , $save ) = $link->save ( $CGI );
								$index      = $link->reindex ( $stoplist );
								$categorize = $link->categorize ( $cats );
								$dump       = $link->dump_data ();
								$xxxx       = $link->show_debug_info ();
								$output     = "
                   <b>$save</b>
                   <hr width='75%'>
                   $dump
                   <hr width='75%'>
                   <b>$language[link_save_recategorize]</b><br />$categorize
                   <hr width='75%'>
                   <b>$language[link_save_reindex]</b><br />$index
               ";
						}
						if ( $return ) {
								return $save;
						}
						$OUT = "
              <table align='left' border=0 cellspacing=10 width=98%>
                 <tr>
                    <td valign='top' align='left'>
                     $output
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[link_save_title] <i>$CGI[id] / $link_title</i>" , $OUT );
				}

				#--------------------------------------------------------------------------#
				#  Delete a listing                                                        #
				#--------------------------------------------------------------------------#
				function delete_link ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$link_title = $link->data[ title ];
						$message    = '';
						if ( $moderator && $moderator_categories[ $link->data[ category ] ][ can_delete_listings ] != "Y" ) {
								$link                          = new Listing( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
								$temp->data[ submission_type ] = "MOD_DELETE";
								$temp->data[ submitted_by ]    = $moderator[ acct_id ];
								$temp->update_values ( $temp->data );
								$temp->save ();
								$message .= $language[ moderator_cant_delete ];
						} else {
								## Log the deletion for this moderator
								if ( $moderator && $moderator_categories[ $link->data[ category ] ][ can_delete_listings ] == "Y" ) {
										$mod_id = $moderator[ acct_id ];
										$cat_id = $link->data[ category ];
										$message .= $this->log_moderator_activity ( array ( "moderator_id" => $mod_id , "category_id" => $cat_id , "action" => 'delete' ) );
								}
								$message .= $link->delete ( "hyperseek" );
						}
						$OUTPUT = "
              <table align='left' border='0' cellspacing='10' width='98%'>
                 <tr>
                    <td valign='top' align='left'>
                       <b>$language[link_delete_header]</b><br />$message
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[link_delete_title] <i>$CGI[id] / $link_title</i>" , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  Send listing owner an email                                             #
				#--------------------------------------------------------------------------#
				function email_link ()
				{
						$link  = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$email = $link->data[ email ];
						header ( "Location: mailto: $email" );
						exit;
				}

				#--------------------------------------------------------------------------#
				#  Report on Duplicate listing URLs                                        #
				#--------------------------------------------------------------------------#
				function duplicate_listings ()
				{
						global $oSQL;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						$domains = array ();
						## Using a normal DBI Handle, so that we can iterate through the list
						## without having to load it all into memory.
						$SQL = "SELECT id, url FROM " . LINKS_TABLE;
						$qid = $oSQL->run ( $SQL );
						while ( $row = $oSQL->fetch_assoc ( $qid ) ) {
								$ID  = $row[ id ];
								$URL = $row[ url ];
								if ( !preg_match ( "/(news|mailto)\:/" , $URL ) ) {
										$domains[ 'absolute' ][ $URL ]++;
										$urlinfo = parse_url ( $URL );
										$host    = $urlinfo[ 'host' ];
										$domains[ 'domain' ][ $host ]++;
								}
						}
						$num_abs = 0;
						$num_dom = 0;
						foreach ( $domains[ 'absolute' ] as $domain => $count ) {
								if ( $count <= 1 ) {
										continue;
								}
								$enc_absolute = urlencode ( $domain );
								$num_abs++;
								$abslist .= " <li><a href='$SCRIPT_NAME?admin=hyperseek&action=search&Terms=url:$enc_absolute'>$domain</a> ($count)</li>";
						}
						foreach ( $domains[ 'domain' ] as $domain => $count ) {
								if ( $count <= 1 ) {
										continue;
								}
								$num_dom++;
								$enc_domain = urlencode ( $domain );
								$domlist .= " <li><a href='$SCRIPT_NAME?admin=hyperseek&action=search&Terms=url:$enc_domain'>$domain</a> ($count)</li>";
						}
						$OUTPUT = "
              <table bgcolor='#e0e0e0' align='left' border='1' cellspacing='0' cellpadding='10'>
                 <tr bgcolor='silver'>
                    <td valign='top' align='left' colspan='2'>
                       <b>$language[duplicate_links_title]</b>
                    </td>
                 </tr>
                 <tr bgcolor='silver'>
                    <th>$language[duplicate_links_exact] $num_abs</th>
                    <th>$language[duplicate_links_partial] $num_dom</th>
                 </tr>
                 <tr>
                    <td valign='top'>
                       $abslist
                    </td>
                    <td valign='top'>
                       $domlist
                    </td>
                 </tr>
              </table>

           ";
						admin_output ( "$language[duplicate_links_title]" , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  Database Mass Deletion of listings                                      #
				#--------------------------------------------------------------------------#
				function mass_delete ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$output = '';
						if ( $CGI[ mass_delete ] ) {
								$text    = $CGI[ delete_text ];
								$field   = '';
								$deleted = 0;
								$errors  = 0;
								$total   = 0;
								$output .= "$language[massdelete_updating]<hr />\n";
								if ( $CGI[ 'delete_field' ] == "Category" ) {
										$field = "category";
								}
								if ( $CGI[ 'delete_field' ] == "URL" ) {
										$field = "url";
								}
								if ( $CGI[ 'delete_field' ] == "Keys" ) {
										$field = "keywords";
								}
								if ( $CGI[ 'delete_field' ] == "Email" ) {
										$field = "email";
								}
								if ( $CGI[ 'delete_field' ] == "Title" ) {
										$field = "title";
								}
								if ( $CGI[ 'delete_field' ] == "Description" ) {
										$field = "description";
								}
								$SQL = "SELECT id FROM " . LINKS_TABLE . " where $field LIKE '%$text%'";
								$ids = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
								## Do it ....
								foreach ( $ids as $linkid ) {
										$link = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , Lookup => 1 , id => $linkid ) );
										if ( $link->delete () ) {
												$output .= "-";
												$deleted++;
										} else {
												$output .= ".";
												$errors++;
										}
										## Clean up screen.
										$total++;
										if ( $total == 50 ) {
												print "<br />";
												$total = 0;
										}
								}
								$list = implode ( "," , $ids );
								$output .= "
                   $language[massdelete_done]
                   <br /><br />
                   $deleted $language[massdelete_deleted]
                   <br />
                   $errors $language[massdelete_errors].
                   <br /><br />
                ";
						} else {
								$output = "
                    <br /><br />
                    <form name='msg_form' method='post' enctype='x-www-form-urlencoded' action='$script_name'>
                        <u><b>Mass Deletion of Links</b></u><br />
                        Delete all links that contain: <input name='delete_text'><br /> in this field:
                        <select name='delete_field'>
                         <option value='Category'>$language[massdelete_category]
                         <option value='URL'>$language[massdelete_url]
                         <option value='Email'>$language[massdelete_email]
                         <option value='Title'>$language[massdelete_title]
                         <option value='Description'>$language[massdelete_description]
                         <option value='Keys'>$language[massdelete_keys]
                        </select>
                        <input type='hidden' name='action' value='mass_delete'>
                        <input type='hidden' name='admin' value='hyperseek'>
                        <input type='hidden' name='mass_delete' value='1'>
                        <input type='submit' value='$language[massdelete_button]'>
                    </form>
                ";
						}
						admin_output ( $language[ massdelete_header ] , $output );
				}

				#--------------------------------------------------------------------------#
				#  Database Export                                                         #
				#--------------------------------------------------------------------------#
				function export_listings ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $data_dir;
						global $SCRIPT_NAME;
						if ( $CGI[ export_listings ] ) {
								print "$language[export_listings_header]<br />";
								$count = 0;
								$total = 0;
								$char  = '';
								if ( $CGI[ format ] == "pipe" ) {
										$char = "|";
								} elseif ( $CGI[ format ] == "tab" ) {
										$char = "\t";
								} elseif ( $CGI[ format ] == "csv" ) {
										$char = '","';
								}
								$file     = "$data_dir/hyperseek/hyperseek." . $CGI[ format ];
								$bidsfile = "$data_dir/hyperseek/bids." . $CGI[ format ];
								$fh       = fopen ( $file , 'w+' );
								if ( $fh ) {
										if ( flock ( $fh , LOCK_EX ) ) {
												## Not using objects here, so that we can iterate through this like a cursor.
												$SQL = "SELECT * FROM " . LINKS_TABLE;
												$qid = $oSQL->run ( $SQL );
												while ( $row = $oSQL->fetch_row ( $qid ) ) {
														$line = implode ( $char , $row );
														if ( $CGI[ format ] == "csv" ) {
																$line = '"' . $line . '"';
														}
														$line = preg_replace ( "/(\r|\n|\cM)/" , "" , $line );
														$line .= "\n";
														fwrite ( $fh , $line );
														$total++;
														$count++;
														echo ".";
														if ( $total == 50 ) {
																$total = 0;
																echo "<br />\n";
														}
												}
										} else {
												print "Error Opening $file for write<br />";
										}
								} else {
										print "Error Opening $file for write<br />";
								}
								$language[ export_listings_summary ] = preg_replace ( '/(<<count>>)/' , $count , $language[ export_listings_summary ] );
								print "<br /><br />$language[export_listings_summary] $file<br /><br />\n";
								$count = 0;
								$total = 0;
								## Now, save the bids ....
								$fh = fopen ( $bidsfile , 'w+' );
								if ( $fh ) {
										if ( flock ( $fh , LOCK_EX ) ) {
												## Not using objects here, so that we can iterate through this like a cursor.
												$SQL = "SELECT * FROM " . KEYS_TABLE;
												$qid = $oSQL->run ( $SQL );
												while ( $row = $oSQL->fetch_row ( $qid ) ) {
														$line = implode ( $char , $row );
														if ( $CGI[ format ] == "csv" ) {
																$line = '"' . $line . '"';
														}
														$line = preg_replace ( "/(\r|\n|\cM)/" , "" , $line );
														$line .= "\n";
														fwrite ( $fh , $line );
														$total++;
														$count++;
														echo ".";
														if ( $total == 50 ) {
																$total = 0;
																echo "<br />\n";
														}
												}
										} else {
												print "Error Opening $bidsfile for write<br />";
										}
								} else {
										print "Error Opening $bidsfile for write<br />";
								}
								$language[ export_keys_summary ] = preg_replace ( '/(<<count>>)/' , $count , $language[ export_keys_summary ] );
								print "<br /><br />$language[export_keys_summary] $bidsfile<br /><br />\n";
								exit;
						} else {
								$OUTPUT = "
                <form action = '$SCRIPT_NAME' method='post'>
                   <h2>$language[export_listings_title]</h2>
                   $language[export_format]
                   <select name='format'>
                     <option value='pipe'>$language[export_listings_pipe]</option>
                     <option value='csv'>$language[export_listings_csv]</option>
                     <option value='tab'>$language[export_listings_tab]</option>
                   </select>
                   <input type='hidden' name='action' value='export_listings'>
                   <input type='hidden' name='admin' value='hyperseek'>
                   <input type='hidden' name='export_listings' value='1'>
                   <input type='submit' value='$language[export_listings_button]'>
                </form>
             ";
						}
						admin_output ( $language[ export_listings_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Database Import                                                         #
				#--------------------------------------------------------------------------#
				function import_listings ()
				{
						global $CGI;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $UPLOADS;
						global $data_dir;
						$filename = '';
						$output   = '';
						if ( $CGI[ filename ] ) {
								$filename = $CGI[ filename ];
						} elseif ( $UPLOADS[ 'import_file' ][ 'name' ] ) {
								// Do uploading stuff ... set the new filename here //
								$uploaded_file = $UPLOADS[ 'import_file' ][ 'tmp_name' ];
								$filename      = $data_dir . "/hyperseek/uploaded.dat";
								copy ( $uploaded_file , $filename );
								$CGI[ 'filename' ] = $filename;
						}
						if ( $CGI[ step ] == "2" && ( $filename && file_exists ( $filename ) ) ) {
								$this->import_step_2 ();
						} elseif ( $CGI[ step ] == "3" ) {
								$this->import_step_3 ();
						} else {
								$FORM = "
                  <table align='left' border=0 cellspacing=10>
                     <tr>
                        <td valign='top' align='left'>

                            <form name='hsform' action='$SCRIPT_NAME' method='post' enctype='multipart/form-data'>

                            $language[import_listings_upload_prompt]<input type='file' name='import_file'>
                            <br /> ... or ... <br />

                            Enter Server Path to file for import: <input name='filename' value='$filename'>
                            <br /><br />

                            $language[import_listings_format]<select name='format'>
                            <option value='csv'>$language[export_listings_csv]
                            <option value='tab'>$language[export_listings_tab]
                            <option value='pipe'>$language[export_listings_pipe]
                            </select>
                            <input type='hidden' name='action' value='import_listings'>
                            <input type='hidden' name='admin' value='hyperseek'>
                            <input type='hidden' name='step' value='2'>
                            <input type='hidden' name='dummy' value='x'>
                            <br /><br />
                            <input type='submit' value='$language[import_listings_button]'>
                            </form>
                        </td>
                     </tr>
                  </table>

               ";
								admin_output ( "$language[import_listings_title]: 1" , $FORM );
						}
				}

				function import_step_2 ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						$filename  = $CGI[ filename ];
						$separator = '';
						if ( $CGI[ format ] == "csv" ) {
								$separator = "\",\"";
						} elseif ( $CGI[ format ] == "pipe" ) {
								$separator = "\\|";
						} elseif ( $CGI[ format ] == "tab" ) {
								$separator = "\t";
						}
						$fieldlist   = "
             <option value=''>$language[import_listings_select]
             <option value='ID'>$language[import_listings_field_id]
             <option value='Category'>$language[import_listings_field_category]
             <option value='Extra Link'>$language[import_listings_field_extra_link]
             <option value='Rank'>$language[import_listings_field_rank]
             <option value='Qualifier'>$language[import_listings_field_qualifier]
             <option value='Email'>$language[import_listings_field_email]
             <option value='Title'>$language[import_listings_field_title]
             <option value='URL'>$language[import_listings_field_url]
             <option value='Description'>$language[import_listings_field_description]
             <option value='Keywords'>$language[import_listings_field_keywords]
             <option value='Name'>$language[import_listings_field_name]
             <option value='Address'>$language[import_listings_field_address]
             <option value='City'>$language[import_listings_field_city]
             <option value='State'>$language[import_listings_field_state]
             <option value='Zip'>$language[import_listings_field_zip]
             <option value='Country'>$language[import_listings_field_country]
             <option value='Phone'>$language[import_listings_field_phone]
             <option value='Fax'>$language[import_listings_field_fax]
             <option value='Startdate'>$language[import_listings_field_startdate]
             <option value='Enddate'>$language[import_listings_field_enddate]
             <option value='Price'>$language[import_listings_field_price]
             <option value='Account'>$language[import_listings_field_account]
             <option value='Xfactor'>$language[import_listings_field_xfactor]
             <option value='Siteicon'>$language[import_listings_field_siteicon]
             <option value='Descicon'>$language[import_listings_field_descicon]
             <option value='Titletag'>$language[import_listings_field_titletag]
             <option value='Field1'>$language[import_listings_field_field1]
             <option value='Field2'>$language[import_listings_field_field2]
             <option value='Field3'>$language[import_listings_field_field3]
             <option value='Field4'>$language[import_listings_field_field4]
             <option value='Field5'>$language[import_listings_field_field5]
             <option value='Field6'>$language[import_listings_field_field6]
             <option value='Field7'>$language[import_listings_field_field7]
             <option value='Field8'>$language[import_listings_field_field8]
             <option value='Field9'>$language[import_listings_field_field9]
             <option value='Field10'>$language[import_listings_field_field10]
           ";
						$contents    = file ( $filename );
						$num_records = count ( $contents );
						$LINE        = $contents[ 0 ];
						$LINE        = preg_replace ( "/^\"|\"$/" , "" , $LINE );
						$x           = 0;
						$input       = '';
						$fields      = preg_split ( "/$separator/" , $LINE );
						foreach ( $fields as $field ) {
								$inputname = "field$x";
								$input .= " <tr><td>$field</td><td><select name='$inputname'>$fieldlist</select></td></tr> ";
								$x++;
						}
						$OUTPUT = "
              <table align='left' border='0' cellspacing='10' width='400'>
                 <tr>
                    <td colspan=2>
                       $language[import_listings_examined] $filename<br />
                       $language[import_listings_delimiter] $cgi[format]<br />
                       $language[import_listings_total] $num_records
                    </td>
                  </tr>

                  <tr>

                    <td valign='top' align='left'>

                        <form name='hsform' action='$SCRIPT_NAME' method='post'>

                            <table border=1 bgcolor='#e0e0e0'>
                               <tr bgcolor='black'>
                                <th><font color='white'>$language[import_listings_data_column]</font></th>
                                <th><font color='white'>$language[import_listings_field_column]</font></th>
                               </tr>
                               <tr>
                                 <td colspan=2>
                                 $language[import_listings_howto]
                                 </td>
                               </tr>
                                 $input
                               <tr>
                                 <td colspan=2 align='center'>
                                  <input type='hidden' name='num_fields' value='$x'>
                                  <input type='hidden' name='action' value='import_listings'>
                                  <input type='hidden' name='admin' value='hyperseek'>
                                  <input type='hidden' name='step' value='3'>
                                  <input type='hidden' name='filename' value='$filename'>
                                  <input type='hidden' name='format' VALUE='$CGI[format]'>
                                  <input type='submit' value='$language[import_listings_continue_button]'>
                                 </td>
                               </tr>
                             </table>

                         </form>
                       </td>
                  </tr>
               </table>

           ";
						admin_output ( "$language[import_listings_title] 2" , $OUTPUT );
				}

				function import_step_3 ()
				{
						global $CGI;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						global $oSQL;
						global $data_dir;
						$converted_filename = "$data_dir/hyperseek/hyperseek.import";
						$filename           = $CGI[ 'filename' ];
						$num_fields         = $CGI[ 'num_fields' ];
						$format             = $CGI[ 'format' ];
						$separator          = '';
						$output             = '';
						$SQL                = "SELECT category_id, full_name FROM " . CATEGORY_TABLE;
						$catinfo            = $oSQL->search ( array ( "SQL" => $SQL ) );
						$categories         = array ();
						foreach ( $catinfo as $cat ) {
								$categories[ $cat[ full_name ] ] = $cat[ category_id ];
						}
						if ( $format == "csv" ) {
								$separator = "\",\"";
						} elseif ( $format == "pipe" ) {
								$separator = "\\|";
						} elseif ( $format == "tab" ) {
								$separator = "\t";
						}
						$ID_Position          = 99;
						$Category_Position    = 99;
						$Extra_Position       = 99;
						$Rank_Position        = 99;
						$Qualifier_Position   = 99;
						$Email_Position       = 99;
						$Title_Position       = 99;
						$URL_Position         = 99;
						$Description_Position = 99;
						$Keywords_Position    = 99;
						$Name_Position        = 99;
						$Address_Position     = 99;
						$City_Position        = 99;
						$State_Position       = 99;
						$Zip_Position         = 99;
						$Country_Position     = 99;
						$Phone_Position       = 99;
						$Fax_Position         = 99;
						$Startdate_Position   = 99;
						$Enddate_Position     = 99;
						$Price_Position       = 99;
						$Account_Position     = 99;
						$Xfactor_Position     = 99;
						$Siteicon_Position    = 99;
						$Descicon_Position    = 99;
						$Titletag_Position    = 99;
						$Field1_Position      = 99;
						$Field2_Position      = 99;
						$Field3_Position      = 99;
						$Field4_Position      = 99;
						$Field5_Position      = 99;
						$Field6_Position      = 99;
						$Field7_Position      = 99;
						$Field8_Position      = 99;
						$Field9_Position      = 99;
						$Field10_Position     = 99;
						for ( $x = 0; $x <= $num_fields; $x++ ) {
								$fieldname = "field$x";
								if ( $CGI[ $fieldname ] == "ID" ) {
										$ID_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Extra Link" ) {
										$Extra_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Category" ) {
										$Category_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Rank" ) {
										$Rank_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Qualifier" ) {
										$Qualifier_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Email" ) {
										$Email_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Title" ) {
										$Title_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "URL" ) {
										$URL_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Description" ) {
										$Description_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Keywords" ) {
										$Keywords_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Name" ) {
										$Name_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Address" ) {
										$Address_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "City" ) {
										$City_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "State" ) {
										$State_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Zip" ) {
										$Zip_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Country" ) {
										$Country_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Phone" ) {
										$Phone_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Fax" ) {
										$Fax_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Startdate" ) {
										$Startdate_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Enddate" ) {
										$Enddate_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Price" ) {
										$Price_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Account" ) {
										$Account_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Xfactor" ) {
										$Xfactor_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Siteicon" ) {
										$Siteicon_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Descicon" ) {
										$Descicon_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Titletag" ) {
										$Titletag_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field1" ) {
										$Field1_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field2" ) {
										$Field2_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field3" ) {
										$Field3_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field4" ) {
										$Field4_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field5" ) {
										$Field5_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field6" ) {
										$Field6_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field7" ) {
										$Field7_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field8" ) {
										$Field8_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field9" ) {
										$Field9_Position = $x;
								}
								if ( $CGI[ $fieldname ] == "Field10" ) {
										$Field10_Position = $x;
								}
						}
						$y             = time ();
						$converted     = 0;
						$imported      = file ( $filename );
						$imported_data = '';
						foreach ( $imported as $LINE ) {
								$converted++;
								$LINE   = preg_replace ( "/(\r|\n|\cM)/" , "" , $LINE );
								$LINE   = preg_replace ( "/^\"|\"$/" , "" , $LINE );
								$fields = preg_split ( "/$separator/" , $LINE );
								$ttl    = count ( $fields );
								for ( $x = 0; $x <= $ttl; $x++ ) {
										$fields[ $x ] = preg_replace ( "/\|/" , "" , $fields[ $x ] );
								}
								$ID = $fields[ $ID_Position ];
								if ( !$ID || $ID_Position == 99 ) {
										$y++;
										$ID = $y;
								}
								$Category     = $fields[ $Category_Position ];
								$RealCategory = $categories[ $Category ];
								if ( !$RealCategory ) {
										$RealCategory = $Category;
								}
								$ELink       = $fields[ $Extra_Position ];
								$Position    = $fields[ $Rank_Position ];
								$Qualifier   = $fields[ $Qualifier_Position ];
								$Email       = $fields[ $Email_Position ];
								$Title       = $fields[ $Title_Position ];
								$URL         = $fields[ $URL_Position ];
								$Description = $fields[ $Description_Position ];
								$Keys        = $fields[ $Keywords_Position ];
								$Name        = $fields[ $Name_Position ];
								$Address     = $fields[ $Address_Position ];
								$City        = $fields[ $City_Position ];
								$State       = $fields[ $State_Position ];
								$Zip         = $fields[ $Zip_Position ];
								$Country     = $fields[ $Country_Position ];
								$Phone       = $fields[ $Phone_Position ];
								$Fax         = $fields[ $Fax_Position ];
								$startdate   = $fields[ $Startdate_Position ];
								$enddate     = $fields[ $Enddate_Position ];
								$price       = $fields[ $Price_Position ];
								$account     = $fields[ $Account_Position ];
								$xfactor     = $fields[ $Xfactor_Position ];
								$site_icon   = $fields[ $Siteicon_Position ];
								$desc_image  = $fields[ $Descicon_Position ];
								$title_tag   = $fields[ $Titletag_Position ];
								$field1      = $fields[ $Field1_Position ];
								$field2      = $fields[ $Field2_Position ];
								$field3      = $fields[ $Field3_Position ];
								$field4      = $fields[ $Field4_Position ];
								$field5      = $fields[ $Field5_Position ];
								$field6      = $fields[ $Field6_Position ];
								$field7      = $fields[ $Field7_Position ];
								$field8      = $fields[ $Field8_Position ];
								$field9      = $fields[ $Field9_Position ];
								$field10     = $fields[ $Field10_Position ];
								$newline     = "$ID|$RealCategory|$ELink|$Position|$Qualifier|$Email|$Title|$URL|$Description|$Keys|$desc_image|$title_tag|$site_icon|$Name|$Address|$City|$State|$Zip|$Country|$Phone|$Fax|$startdate|$enddate|$price|$account|$xfactor|$field1|$field2|$field3|$field4|$field5|$field6|$field7|$field8|$field9|$field10\n";
								$imported_data .= $newline;
						}
						write_file ( $converted_filename , $imported_data );
						$x               = 0;
						$validate_import = file ( $converted_filename );
						foreach ( $validate_import as $LINE ) {
								$x++;
								if ( $x >= 10 ) {
										break;
								}
								list ( $ID , $Category , $ELink , $Position , $Qualifier , $Email , $Title , $URL , $Description , $Keys , $desc_image , $title_tag , $site_icon , $Name , $Address , $City , $State , $Zip , $Country , $Phone , $Fax , $startdate , $enddate , $price , $account , $xfactor , $field1 , $field2 , $field3 , $field4 , $field5 , $field6 , $field7 , $field8 , $field9 , $field10 ) = preg_split ( '/\|/' , $LINE );
								$output .= "<b>ID</b>: $ID<br />\n";
								$output .= "<b>Category</b>: $Category<br />\n";
								$output .= "<b>Email</b>: $Email<br />\n";
								$output .= "<b>Title</b>: $Title<br />\n";
								$output .= "<b>URL</b>: $URL<br />\n";
								$output .= "<b>Description</b>: $Description<br />\n";
								$output .= "<b>Keywords</b>: $Keys<br /><br />\n";
						}
						$FORM = "
              <table align='left' border='0' cellspacing='10' width='400'>
                 <tr>
                    <td valign='top' align='left'>
                        <b>$filename: $converted $language[import_listings_total]</b><br /><br />

                        $language[import_listings_review]
                        <hr />

                        $output
                        <form name='hsform' action='$SCRIPT_NAME' method='post'>
                        <input type='hidden' name='admin' value='hyperseek'>
                        <input type='hidden' name='action' value='finish_import'>
                        <input type='submit' value='$language[import_listings_finish_button]'>
                        </form>
                    </td>
                 </tr>
              </table>

           ";
						admin_output ( "$language[import_listings_title] 3" , $FORM );
				}

				function finish_import ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $data_dir;
						$report = '';
						$count  = 0;
						## Master fieldname Array
						$fieldnames = array ( 'id' , 'category' , 'extra_link' , 'rank' , 'qualifier' , 'email' , 'title' , 'url' , 'description' , 'keywords' , 'desc_image' , 'title_tag' , 'site_icon' , 'name' , 'address' , 'city' , 'state' , 'zip' , 'country' , 'phone' , 'fax' , 'startdate' , 'enddate' , 'price' , 'account' , 'xfactor' , 'field1' , 'field2' , 'field3' , 'field4' , 'field5' , 'field6' , 'field7' , 'field8' , 'field9' , 'field10' );
						$importlist = file ( "$data_dir/hyperseek/hyperseek.import" );
						foreach ( $importlist as $listing ) {
								$listing     = preg_replace ( "/(\r|\n|\cM)/" , "" , $listing );
								$listingdata = explode ( '|' , $listing );
								$linkdata    = dsx_array_combine ( $fieldnames , $listingdata );
								if ( $linkdata[ id ] ) {
										$record = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $linkdata[ id ] ) );
										## Assign and Save the data ...
										if ( $record->update_values ( $linkdata ) ) {
												if ( $record->save () ) {
														$record->reindex ( $stoplist );
														$cats = array ();
														foreach ( split ( ";" , $linkdata[ category ] ) as $cat ) {
																array_push ( $cats , $cat );
														}
														$record->categorize ( $cats );
														print "$language[link_save_ok]: (ID: $linkdata[id]: Title: $linkdata[title]) <br />\n";
														$count++;
												} else {
														print "$language[link_save_error]: $linkdata{title} <pre>$record->error_text</pre> <br />\n";
												}
										} else {
												print "$language[link_save_error]: $linkdata{title} <pre>$record->error_text</pre> <br />\n";
										}
								}
						}
						echo "<h3>Import Complete: $count records imported</h3>";
						exit;
				}

				function spider_form ()
				{
						global $language;
						$FORM = "

              <script language='javascript'>
                 function changeCategory() { X=window.open('$SCRIPT_NAME?admin=hyperseek&action=category_option_form','X','toolbar=no,directories=no,menubar=no,scrollbars=yes,resizable=yes,width=400,height=350'); }
              </script>

              <table width=400>
                <tr>
                  <td nowrap>
                    <form name='input_form' action='$SCRIPT_NAME' method='post'>
                    URL(s) TO SPIDER...<br />
                    <textarea name='url' rows=10 cols=50></textarea>
                    <br /><br />
                    <a href='javascript:changeCategory()'>$language[spider_category_prompt]</a>
                    <input name='category'>
                    <br /><br />
                    $language[spider_database_prompt]
                    <select name='target'>
                        <option value='live'>$language[spider_database_live]</option>
                        <option value='temp'>$language[spider_database_temp]</option>
                    </select>
                    <br /><br />
                    <center>
                        <input type='hidden' name='action' value='spider'>
                        <input type='hidden' name='admin' value='hyperseek'>
                        <input type='submit' value='$language[spider_button]'>
                    </center>
                    </form>
                  </td>
                </tr>
              </table>
           ";
						admin_output ( $language[ spider_title ] , $FORM );
				}

				function mass_spider ()
				{
						global $data_dir;
						global $CGI;
						$urls  = array ();
						$links = file ( "$data_dir/hyperseek/spiderus.txt" );
						foreach ( $links as $listing ) {
								$listing = preg_replace ( "/(\r|\n|\cM)/" , "" , $listing );
								list ( $url , $cat ) = split ( "|" , $listing );
								// Set the category for saving //
								$CGI[ 'category' ] = $cat;
								echo "Spidering $url<br />\n";
								flush ();
								$this->spider ( $url );
						}
						echo "<h2>DONE</h2>\n";
				}

				function spider ( $sent_url = '' )
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						$output = '';
						## Defaults
						$CGI[ id ]   = time ();
						$CGI[ rank ] = $config[ Default_Rank ];
						if ( $sent_url ) {
								$CGI[ url ] = $sent_url;
						}
						$urllist    = $CGI[ url ];
						$CGI[ url ] = '';
						foreach ( preg_split ( "/\s+/" , $urllist ) as $url ) {
								$metatags = get_meta_tags ( $url , 1 );
								$content  = implode ( "" , file ( $url ) );
								preg_match ( "/\<title\>(.*)\<\/title\>/i" , $content , $tmatch );
								$CGI[ title ]       = $tmatch[ 1 ];
								$CGI[ keywords ]    = $metatags[ keywords ];
								$CGI[ description ] = $metatags[ description ];
								$CGI[ id ]++;
								$CGI[ url ] = $url;
								$ouput .= "
                 ID: $CGI[id]<br />
                 URL: $CGI[url]<br />
                 Title: $CGI[title]<br />
                 Desc: $CGI[description]<br />
                 Keys: $CGI[keywords]<br />
              ";
								if ( $CGI[ target ] == "temp" ) {
										$link                   = new Listing( array ( "Parent" => $iSQL , "Table" => TEMP_TABLE , "Schema" => TEMP_TABLE ) );
										$CGI[ submission_type ] = "SPIDERED";
										$CGI[ submitted_by ]    = "SPIDER";
										if ( $link->update_values ( $CGI ) ) {
												if ( $link->save () ) {
														$output .= $language[ link_save_ok ] . "<br />";
												} else {
														$output .= "$language[link_save_error] <pre>$link->error_text</pre>";
												}
										} else {
												$output .= "$language[link_save_error] <pre>$link->error_text</pre>";
										}
								} else {
										$link  = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE ) );
										$save  = $link->save ( $CGI );
										$index = $link->reindex ( $stoplist );
										$cats  = array ();
										foreach ( split ( ";" , $CGI[ category ] ) as $cat ) {
												array_push ( $cats , $cat );
										}
										$categorize = $link->categorize ( $cats );
										$dump       = $link->dump_data ();
										$xxxx       = $link->show_debug_info ();
										$output .= "
                       <b>$language[link_save_ok]</b>:
                       <br />$save
                       <hr width='75%'>
                       $dump
                       <hr width='75%'>
                       <b>$language[link_save_recategorize]</b><br />$categorize
                       <hr width='75%'>
                       <b>$language[link_save_reindex]</b><br />$index
                  ";
								}
						}
						if ( $sent_url ) {
								return $output;
						}
						$OUTPUT = "
              <table width='400'>
              <tr><td nowrap='nowrap'>
              $output
              </td></tr></table>
           ";
						admin_output ( $language[ spider_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  A list of all the currently defined "featured" sites                    #
				#--------------------------------------------------------------------------#
				function list_featured ()
				{
						global $oSQL;
						global $config;
						global $language;
						global $CGI;
						global $SCRIPT_NAME;
						$report  = '';
						$SQL     = "SELECT * FROM " . FEATURED_TABLE . " ORDER BY area";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $results as $area ) {
								$SQL  = "SELECT * FROM " . LINKS_TABLE . " WHERE id = $area[id]";
								$link = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
								$SQL  = "SELECT * FROM " . CATEGORY_TABLE . " WHERE category_id = '$area[area]'";
								$cat  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
								if ( !$cat[ full_name ] ) {
										$cat[ full_name ] = $area{area};
								}
								$report .= "
                 <tr>
                   <td>$cat[full_name]</td>
                   <td>$link[title]</td>
                   <td>
                      [
                      <a href='$link[url]' TARGET='new'>$language[featured_visit]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]'>$language[featured_edit]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_featured&area=$area[area]'>$language[featured_unfeature]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=feature_link&area=default&id=$link[id]'>$language[featured_global]</a>
                      ]
                   </td>
                 </tr>
              ";
						}
						$OUTPUT = "
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

                <table cellspacing='0' onclick='sortColumn(event)' class='sorted'>
                <thead>
                    <tr>
                        <td style='width: 160px;'>$language[featured_category]</td>
                        <td style='width: 160px;'>$language[featured_listing]</td>
                        <td style='width: 160px;'>$language[featured_action]</td>
                    </tr>
                </thead>
                <tbody>
                   $report
                </tbody>
                </table>
           ";
						admin_output ( $language[ featured_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Make a site "featured" for the current cateogry or search term          #
				#--------------------------------------------------------------------------#
				function feature_link ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$link_title = $link->data[ title ];
						$lookup     = $CGI[ category ];
						if ( $CGI[ Terms ] ) {
								$lookup = $CGI[ Terms ];
						}
						if ( !$lookup ) {
								$lookup = "default";
						}
						$message                = '';
						$featured               = new Record( array ( "Parent" => $iSQL , "Table" => FEATURED_TABLE , "Schema" => FEATURED_TABLE , "Lookup" => 1 , "area" => $lookup ) );
						$featured->data[ id ]   = $CGI[ id ];
						$featured->data[ area ] = $lookup;
						if ( $featured->update_values ( $featured->data ) ) {
								if ( $featured->save () ) {
										$message = "<i>$link_title</i> $language[featured_ok]";
								} else {
										$message = "Save Error: <i>$link_title</i> $language[featured_error] <pre>$featured->error_text</pre>";
								}
						} else {
								$message = "Update Error: <i>$link_title</i> $language[featured_error] <pre>$featured->error_text</pre>";
						}
						if ( $CGI[ category ] ) {
								$category = new Record( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category ] ) );
								$message .= " <b>" . $category->data[ full_name ] . "</b>";
						} else {
								$message .= " <b>$CGI[Terms]</b>";
						}
						## Mark this category for the build-changed, if this is a global feature.
						if ( $CGI[ Terms ] == "default" || $CGI[ category ] ) {
								$now   = time ();
								$where = " WHERE category_id = $CGI[category] ";
								if ( $CGI[ Terms ] == "default" ) {
										$where = '';
								}
								$catsql = "UPDATE " . CATEGORY_TABLE . " SET last_activity = $now $where";
								$iSQL->run ( $catsql );
						}
						$OUTPUT = "
              <table align='left' border=0 cellspacing=10 width=98%>
                 <tr>
                    <td valign='top' align='left'>
                       <b>Feature Site Status</b><br />
                       $message
                       <br /><br />
                       <a href='$SCRIPT_NAME?admin=hyperseek&action=feature_link&id=$CGI[id]&Terms=default'>$language[featured_global_link]</a><br /><br />
                       <a href='$SCRIPT_NAME?admin=hyperseek&action=list_featured'>$language[featured_home_link]</a>
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[featured_link_title] <i>$CGI[id] / $link_title</i>" , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Remove "feature" designation for a listing                              #
				#--------------------------------------------------------------------------#
				function delete_featured ()
				{
						global $iSQL;
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$message  = '';
						$featured = new Record( array ( "Parent" => $iSQL , "Table" => FEATURED_TABLE , "Schema" => FEATURED_TABLE , "Lookup" => 1 , "area" => $CGI[ area ] ) );
						if ( $featured->delete () ) {
								$message = "There is now no featured sites for </b>$CGI[area]</b> ";
						} else {
								$message = "There was an error in removing the featured site from <b>$CGI[area]</b> ... <pre>$featured->error_text</pre>";
						}
						$OUTPUT = "
              <table align='left' border=0 cellspacing=10 width=98%>
                 <tr>
                    <td valign='top' align='left'>
                       <b>$language[featured_delete_title]</b><br />$message
                       <br /><br />
                       <a href='$SCRIPT_NAME?admin=hyperseek&action=list_featured'>$language[featured_home_link]</a>
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[featured_delete_title] <i>$CGI[area]</i>" , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  A list of all currently reviewed listings                               #
				#--------------------------------------------------------------------------#
				function review_list ()
				{
						global $oSQL;
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$out = '';
						// Table Vars
						$reviews = REVIEWS_TABLE;
						$links   = LINKS_TABLE;
						$SQL     = "SELECT * FROM $reviews, $links WHERE $reviews.id = $links.id";
						$results = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $results as $link ) {
								$out .= "
                 <tr>
                   <td>$link[title]</td>
                   <td>
                      [
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]'>$language[review_edit_listing]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=review_link&id=$link[id]'>$language[review_edit_review]</a> |
                      <a href='$SCRIPT_NAME?admin=hyperseek&action=save_review&do=delete&id=$link[id]'>$language[review_delete_review]</a>
                      ]
                   </td>
                 </tr>
              ";
						}
						$OUTPUT = "
               <link rel='stylesheet' type='text/css' href='$config[script_dir]/tablesort.css'>
               <script language='javascript' src='$config[script_dir]/tablesort.js'></script>

                <table cellspacing='0' onclick='sortColumn(event)'>
                <thead>
                    <tr>
                        <td style='width: 160px;'>$language[review_listing]</td>
                        <td style='width: 160px;'>$language[review_action]</td>
                    </tr>
                </thead>
                <tbody>
                   $out
                </tbody>
                </table>
           ";
						admin_output ( $language[ review_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Edit a listing's "Review"                                               #
				#--------------------------------------------------------------------------#
				function review_link ()
				{
						global $oSQL;
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$link        = new Listing( array ( "Parent" => $oSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$link_title  = $link->data[ title ];
						$review      = new Record( array ( "Parent" => $oSQL , "Table" => REVIEWS_TABLE , "Schema" => REVIEWS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$review_text = $review->data[ review_text ];
						$OUTPUT      = "
              <table align='left' border=0 cellspacing=10 width=98%>
                 <tr>
                    <td valign='top' align='left'>
                       <b>$language[reviewing_title] $link_title</b><br />
                       <form action='$SCRIPT_NAME' method='post'>
                       <b>$language[review_prompt]</b><br />
                       <textarea name='review_text' rows=15 cols=60>$review_text</textarea>
                       <br />
                       <input type='hidden' name='id' value='$CGI[id]'>
                       <input type='hidden' name='action' value='save_review'>
                       <input type='hidden' name='admin' value='hyperseek'>
                       <input type='submit' value='$language[review_button]'>
                       </form>
                    </td>
                 </tr>
              </table>
           ";
						admin_output ( "$language[reviewing_title] <i>$CGI[id] / $link_title</i>" , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Save a listing's "Review"                                               #
				#--------------------------------------------------------------------------#
				function save_review ()
				{
						global $iSQL;
						global $CGI;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						$link_title = $link->data[ title ];
						$review     = new Record( array ( "Parent" => $iSQL , "Table" => REVIEWS_TABLE , "Schema" => REVIEWS_TABLE , "Lookup" => 1 , "id" => $CGI[ id ] ) );
						if ( $CGI[ 'do' ] == "delete" ) {
								$message = $review->delete () . " (Deleted Review)";
						} else {
								if ( $review->update_values ( $CGI ) ) {
										if ( $review->save () ) {
												$message = "$link_title:  $language[review_saved]<hr />$CGI[review_text]";
										} else {
												$message = "$link_title: $language[review_error] <pre>$review->error_text</pre>";
										}
								} else {
										$message = "$link_title: $language[review_error] <pre>$review->error_text</pre>";
								}
						}
						admin_output ( "$language[reviewing_title] <i>$CGI[id] / $link_title</i>" , $message );
				}





				############################################################################
				#
				#  CATEGORY MODERATION FUNCTIONS
				#
				############################################################################
				#--------------------------------------------------------------------------#
				#  List all cateogry moderators for the current category                   #
				#--------------------------------------------------------------------------#
				function category_moderators ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						// Table Vars
						$mods = MODERATORS_TABLE;
						$cats = CATEGORY_TABLE;
						$SQL  = "SELECT $cats.full_name, $mods.* FROM $cats, $mods WHERE $cats.category_id = $mods.category_id ORDER by category_id, moderator_id";
						$mods = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $mods as $moderator ) {
								$encmod = urlencode ( $moderator[ moderator_id ] );
								$moderator_list .= "
                 <tr>
                    <td style='background:#efefef;border-bottom:1px ridge silver' valign='top'><b><b>$moderator[full_name]</b></b>
                    <td style='background:#efefef;border-bottom:1px ridge silver' valign='top'><b>$moderator[moderator_id]</td>
                    <td style='background:#efefef;border-bottom:1px ridge silver' valign='top'>
                            [
                               <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_moderator&moderator=$encmod&category_id=$moderator[category_id]'>$language[moderator_permissions]</a> |
                               <a href='$SCRIPT_NAME?admin=hyperseek&action=moderator_stats&moderator_id=$encmod&category_id=$moderator[category_id]'>$language[moderator_stats]</a> |
                               <a href='$SCRIPT_NAME?admin=accounts&action=edit_account&acct_id=$encmod'>$language[moderator_profile]</a>
                            ]
                    </td>
                  </tr>
              ";
						}
						$OUTPUT = "
              <table cellspacing='0' cellpadding='5' style='border:1px solid black'>
                <tr style='background:black; color:white'>
                  <th>Category</th>
                  <th>Moderator</th>
                  <th>Actions</th>
                </tr>
                $moderator_list
              </table>
           ";
						print admin_output ( "$language[moderator_title] <i>$name</i>" , $OUTPUT );
				}

				function list_moderators ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						if ( $moderator && ( ( $moderator_categories[ $CGI[ category_id ] ][ access_level ] != "Admin" ) || ( $moderator_categories[ $CGI[ category_id ] ][ access_level ] == "Admin" && $moderator_categories[ $CGI[ category_id ] ][ can_add_moderators ] != "Y" ) )
						) {
								die( $language[ moderator_cant_manage ] );
						}
						if ( !$CGI[ category_id ] ) {
								$CGI[ category_id ] = 0;
						}
						$category       = new CategoryNavigator( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						$name           = $category->data[ full_name ];
						$menu           = category_menu ( $name );
						$moderator_list = '';
						$moderators     = $category->moderators ();
						foreach ( $moderators as $moderator ) {
								$encmod = urlencode ( $moderator[ moderator_id ] );
								$moderator_list .= "
                 $moderator[moderator_id]:
                    <li>
                        [ <a href='$SCRIPT_NAME?admin=hyperseek&action=moderator_stats&moderator_id=$encmod&category_id=$moderator[category_id]'>$language[moderator_stats]</a> |
                        <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_moderator&moderator=$encmod&category_id=$moderator[category_id]'>$language[moderator_permissions]</a> |
                        <a href='$SCRIPT_NAME?admin=accounts&action=edit_account&acct_id=$encmod'>$language[moderator_profile]</a> ]
                        <br /><br />
                    </li>
              ";
						}
						$OUTPUT = "
              <table border=0 cellspacing=20>
               <tr>
                <td valign='top'>
                   <h3>$language[moderator_title]: $name</h3>
                   <ol>
                   $moderator_list
                   </ol>
                </td>
               </tr>
               <tr>
                 <td align='center'>
                    <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_moderator&category_id=$CGI[category_id]'>$language[moderator_add_link]</a>
                 </td>
               </tr>
              </table>
           ";
						print admin_output ( "$language[moderator_title] <i>$name</i>" , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Edit a selected moderator's permissions                                 #
				#--------------------------------------------------------------------------#
				function edit_moderator ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$mod                      = new Record( array ( "Parent" => $oSQL , "Table" => MODERATORS_TABLE , "Schema" => MODERATORS_TABLE , "Lookup" => 1 , "moderator_id" => $CGI[ moderator ] , "category_id" => $CGI[ category_id ] ) );
						$mod->data[ category_id ] = $CGI[ category_id ];
						$mod->update_values ( $mod->data );
						$category  = new CategoryNavigator( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						$name      = $category->data[ full_name ];
						$formarray = $mod->generate_form ();
						$form      = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$form .= "<tr><td bgcolor='#e0e0e0'><b>$prompt</b></td><td bgcolor='#c0c0c0'>$fieldinput</td></tr>\n";
						}
						$back = " <a style='color:white' href='$SCRIPT_NAME?admin=hyperseek&action=category_home&category_id=$CGI[category_id]'>$language[moderator_category_link]</a> ";
						if ( !$moderator ) {
								$menu = category_menu ( $back );
						}
						$OUTPUT = "
              <br />
              <table border=0 cellspacing=0 cellpadding=5>

               <form action='$SCRIPT_NAME' method='post'>

               <tr bgcolor='navy'>
                <td valign='top' colspan=2>
                   <font color='white' size=+1><b>$language[moderator_edit_title]</b></font>
                </td>
               </tr>

               $form

               <tr>
                <td valign='top' colspan=2 align='center'>
                   <input type='hidden' name='action' value='save_moderator'>
                   <input type='hidden' name='admin' value='hyperseek'>
                   <input type='submit' value='$language[moderator_edit_button]'>
                </td>
               </tr>

               </form>

              </table>
           ";
						admin_output ( $language[ moderator_edit_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Save a moderator's permissions                                          #
				#--------------------------------------------------------------------------#
				function save_moderator ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$message = '';
						if ( $moderator && $CGI[ moderator_id ] == $moderator[ acct_id ] ) {
								die( "You may not alter your own permissions" );
						}
						$back = " <a style='color:white' href='$SCRIPT_NAME?admin=hyperseek&action=category_home&category_id=$CGI[category_id]'>Category</a> ";
						if ( !$moderator ) {
								$menu = category_menu ( $back );
						}
						$mod = new Record( array ( "Parent" => $iSQL , "Table" => MODERATORS_TABLE , "Schema" => MODERATORS_TABLE , "Lookup" => 1 , "moderator_id" => $CGI[ moderator_id ] , "category_id" => $CGI[ category_id ] ) );
						if ( $mod->update_values ( $CGI ) ) {
								if ( $mod->save () ) {
										$message = $language[ moderator_saved ];
								} else {
										$message = "$language[moderator_save_error] <pre>$mod->error_text</pre>";
								}
						} else {
								$message = "$language[moderator_save_error] <pre>$mod->error_text</pre>";
						}
						$OUTPUT = "
              <table border='0' cellspacing='20'>
               <tr>
                <td valign='top'>
                   $message
                </td>
               </tr>
              </table>
           ";
						admin_output ( "$language[moderator_save_title] <i>$CGI[moderator_id]</i>" , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Moderator activity log                                                  #
				#--------------------------------------------------------------------------#
				function log_moderator_activity ( $args = array () )
				{
						global $language;
						global $iSQL;
						$log                       = new Record( array ( "Parent" => $iSQL , "Table" => MODERATOR_LOG_TABLE , "Schema" => MODERATOR_LOG_TABLE , "Lookup" => 1 , "moderator_id" => $CGI[ moderator ] , "category_id" => $CGI[ category_id ] ) );
						$log->data[ moderator_id ] = $args{moderator_id};
						$log->data[ category_id ]  = $args{category_id};
						if ( $args[ 'action' ] == "add" ) {
								$num                    = $log->data[ num_added ] + 1;
								$log->data[ num_added ] = $num;
						}
						if ( $args[ 'action' ] == "delete" ) {
								$num                      = $log->data[ num_deleted ] + 1;
								$log->data[ num_deleted ] = $num;
						}
						if ( $args[ 'action' ] == "reject" ) {
								$num                       = $log->data[ num_rejected ] + 1;
								$log->data[ num_rejected ] = $num;
						}
						if ( $args[ 'action' ] == "approve" ) {
								$num                       = $log->data[ num_approved ] + 1;
								$log->data[ num_approved ] = $num;
						}
						$log->update_values ( $log->data );
						if ( $log->save () ) {
								return ( "$language[moderator_logged_ok]<br />" );
						} else {
								return ( "$language[moderator_logged_error]<br />" );
						}
				}


				#--------------------------------------------------------------------------#
				#  Show a moderator's statistics                                           #
				#--------------------------------------------------------------------------#
				function moderator_stats ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						$moderatorlog   = MODERATOR_LOG_TABLE;
						$category       = new Record( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						$current        = $category->data[ full_name ];
						$category_sql   = "SELECT * FROM " . MODERATOR_LOG_TABLE . " WHERE moderator_id = '$CGI[moderator_id]' AND category_id = $CGI[category_id]";
						$category_stats = $oSQL->search ( array ( "SQL" => $category_sql , "Return" => 'hash' ) );
						$total_sql      = "
              SELECT
                SUM(num_added) as total_added, SUM(num_deleted) as total_deleted,
                SUM(num_approved) as total_approved, SUM(num_rejected) as total_rejected
                FROM $moderatorlog WHERE moderator_id = '$CGI[moderator_id]' GROUP BY moderator_id
           ";
						$total_stats    = $oSQL->search ( array ( "SQL" => $category_sql , "Return" => 'hash' ) );
						$OUTPUT         = "
              <h4>$language[moderator_stats_title]: <b>$CGI[moderator_id]</b></h4>
              <table border=0 cellspacing=0 cellpadding=5>
                <tr bgcolor='$config[title_color]'>
                  <th><font color='$config[ttxt_color]'>$language[moderator_stats_category]</font></th>
                  <th><font color='$config[ttxt_color]'>$language[moderator_stats_additions]</font></th>
                  <th><font color='$config[ttxt_color]'>$language[moderator_stats_deletions]</font></th>
                  <th><font color='$config[ttxt_color]'>$language[moderator_stats_rejected]</font></th>
                  <th><font color='$config[ttxt_color]'>$language[moderator_stats_approvals]</font></th>
                </tr>
                <tr>
                  <td align='right'><font color='$config[text_color]'>Current: <i>$current</i></font></td>
                  <td align='right'><font color='$config[text_color]'>$category_stats[num_added]</font></td>
                  <td align='right'><font color='$config[text_color]'>$category_stats[num_deleted]</font></td>
                  <td align='right'><font color='$config[text_color]'>$category_stats[num_rejected]</font></td>
                  <td align='right'><font color='$config[text_color]'>$category_stats[num_approved]</font></td>
                </tr>
                <tr>
                  <td align='right'><font color='$config[text_color]'>All Categories</font></td>
                  <td align='right'><font color='$config[text_color]'>$total_stats[total_added]</font></td>
                  <td align='right'><font color='$config[text_color]'>$total_stats[total_deleted]</font></td>
                  <td align='right'><font color='$config[text_color]'>$total_stats[total_rejected]</font></td>
                  <td align='right'><font color='$config[text_color]'>$total_stats[total_approved]</font></td>
                </tr>
                <tr><td colspan=5><hr /></td></tr>
              </table>
              <br /><br />
              <a href='javascript:history.go(-1)'>$language[moderator_stats_back_link]</a>
           ";
						admin_output ( "$language[moderator_stats_title]: <b>$CGI[moderator_id]</b>" , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  Managing Date Range Expiring Listings                                   #
				#--------------------------------------------------------------------------#
				function expired_links ( $return = '' )
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $templates;
						## Setup
						$expired = array ();
						$pending = array ();
						## Now
						$time    = time ();
						$current = date ( "Ymd" );
						## 2 Weeks from now
						$exptime  = $time + ( 86400 * 14 );
						$due_date = date ( "Ymd" , $exptime );
						$SQL      = "SELECT id, title, url, startdate, enddate FROM " . LINKS_TABLE . " WHERE enddate > 0";
						$links    = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $links as $link ) {
								$expdate = $link[ enddate ];
								## Already Expired
								if ( $expdate < $current ) {
										array_push ( $expired , $link[ id ] );
										$expired_list .= "
                     <a href='$link[url]' target='new'>$link[title]</a> ... [ <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]'>$language[expired_edit]</a> ...
                     <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$link[id]'>$language[expired_delete]</a> ]<br />
                 ";
								} ## Due within 2 weeks
								elseif ( $expdate < $due_date ) {
										array_push ( $pending , $link[ id ] );
										$pending_list .= "
                     <a href='$link[url]' target='new'>$link[title]</a> ... [ <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]'>$language[expired_edit]</a> ...
                     <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$link[id]'>$language[expired_delete]</a> ]<br />
                 ";
								}
						}
						if ( $return == "pending" ) {
								return $pending;
						} elseif ( $return == "expired" ) {
								return $expired;
						}
						$email_options = '';
						$SQL           = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'EMAIL'";
						$letters       = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $letters as $email ) {
								$email_options .= " <option value='$email[template_name]'>$email[template_name]</option> ";
						}
						$OUTPUT = "
              <table border=0 cellspacing=20>
               <tr>
                <td valign='top' colspan=2>

                   <form action='$SCRIPT_NAME' method='post'>
                      Select Action:
                          <select name='action'>
                              <option value='email_pending'>$language[expired_email_pending]</option>
                              <option value='email_expired'>$language[expired_email_expired]</option>
                              <option value='delete_expired'>$language[expired_email_delete]</opion>
                          </select>
                          <br />
                          $language[expired_email_select]
                          <select name='email'>
                              $email_options
                          </select>
                          <input type='submit' value='$language[expired_process]'>
                          <input type='hidden' name='admin' value='hyperseek'>
                   </form>
                </td>
               </tr>
               <tr>
                 <th>$language[expired_links_header]</th>
                 <th>$language[expired_pending_header]</th>
               </tr>
               <tr>
                 <td valign='top'>
                    $expired_list
                 </td>
                 <td valign='top'>
                    $pending_list
                 </td>
               </tr>
              </table>
           ";
						print admin_output ( $language[ expired_title ] , $OUTPUT );
				}

				function email_pending () { $this->process_expired (); }

				function email_expired () { $this->process_expired (); }

				function delete_expired () { $this->process_expired (); }

				function process_expired ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $templates;
						$report        = '';
						$tname         = $CGI[ email ];
						$template_code = $templates[ 'hs_admin' ][ 'default' ][ 'EMAIL' ][ $tname ];
						if ( $CGI[ action ] == "delete_expired" ) {
								$expired = $this->expired_links ( "expired" );
								$cnt     = count ( $expired );
								$report .= "$cnt links are being deleted<hr />";
								foreach ( $expired as $id ) {
										$link       = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $id ) );
										$link_title = $link->data[ title ];
										$message    = $link->delete ();
										$report .= "$link_title: $message<br />\n";
								}
						} elseif ( $CGI[ action ] == "email_expired" ) {
								$expired = $this->expired_links ( "expired" );
								$cnt     = count ( $expired );
								$report .= "$cnt $language[expired_being_notified_delete]<hr />";
								foreach ( $expired as $id ) {
										$SQL      = "SELECT * FROM " . LINKS_TABLE . " WHERE id=$id";
										$link     = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
										$template = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $template_code , "values" => $link ) );
										$body     = $template->merge ();
										mail ( $link[ email ] , $CGI[ email ] , $body , "From: $config[email]\r\n" );
										$report .= "$language[expired_sent_to] $link[email] ... <br />\n";
								}
						} elseif ( $CGI[ action ] == "email_pending" ) {
								$pending = $this->expired_links ( "pending" );
								$cnt     = count ( $pending );
								$report .= "$cnt $language[expired_being_notified_pending]<hr />";
								foreach ( $pending as $id ) {
										$SQL      = "SELECT * FROM " . LINKS_TABLE . " WHERE id=$id";
										$link     = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "hash" ) );
										$template = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $template_code , "values" => $link ) );
										$body     = $template->merge ();
										mail ( $link[ email ] , $CGI[ email ] , $body , "From: $config[email]\r\n" );
										$report .= "$language[expired_sent_to] $link[email] ... <br />\n";
								}
						}
						$OUTPUT = "
              <table border='0' cellspacing='20'>
               <tr>
                 <td valign='top' colspan=2>
                   $report
                 </td>
               </tr>
              </table>
           ";
						admin_output ( $language[ expired_title ] , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  Auto Create User / Login Accounts                                       #
				#--------------------------------------------------------------------------#
				function user_accounts ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $templates;
						$split      = chr ( 129 );
						$output     = '';
						$gen        = array ();
						$time       = time ();
						$SQL        = "SELECT count(*) FROM " . LINKS_TABLE;
						$total      = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$SQL        = "SELECT count(*) FROM " . LINKS_TABLE . " WHERE account in('', 'unassigned')";
						$unassigned = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						if ( $CGI[ create_accounts ] ) {
								$report        = '';
								$template_code = '';
								$template_code = $templates[ 'hs_admin' ][ 'default' ][ 'EMAIL' ][ $CGI[ email ] ];
								$catlist       = $CGI[ categories ];
								if ( $catlist ) {
										foreach ( $catlist as $cat ) {
												array_push ( $catlist , $cat );
										}
										$in  = join ( "','" , $catlist );
										$SQL = "SELECT * FROM " . LINKS_TABLE . " WHERE category IN('$in') AND account in('unassigned','')";
								} else {
										$SQL = "SELECT * FROM " . LINKS_TABLE . " WHERE account in('unassigned','')";
								}
								$links = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $links as $link ) {
										$time++;
										$gpass            = base64_encode ( $time );
										$login            = $link[ email ];
										$link[ email ]    = preg_replace ( "/\'/" , "\'\'" , $link[ email ] );
										$link[ password ] = $gpass;
										$link[ login ]    = $login;
										// Modified by Akhilesh on 12/24/2010 added mysql_real_escape_string() for sql injection.
										$SQL   = "SELECT COUNT(*) FROM " . ACCOUNTS_TABLE . " WHERE acct_id = '" . mysql_escape_string ( $login ) . "'";
										$count = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
										$SQL   = "UPDATE " . LINKS_TABLE . " SET account = '$login' WHERE id = '$link[id]'";
										$iSQL->run ( $SQL );
										if ( $count >= 1 ) {
												$report .= "<b>Account $login Exists ... skipping</b><br /><br />\n";
												continue;
										}
										$report .= "
                   $language[accounts_account] $link[login] $language[accounts_created] ...
                   <br />
                   Link URL: $link[url]<br />
                   P/W: $link[password]<br />
                ";
										$account = new Record ( array ( "Parent" => $iSQL , "Table" => ACCOUNTS_TABLE , "Lookup" => 1 , "Schema" => ACCOUNTS_TABLE , "acct_id" => $link[ login ] ) );
										if ( !$link[ name ] ) {
												$link[ name ] = 'N/A';
										}
										$account->data[ acct_id ]       = $link[ login ];
										$account->data[ acct_password ] = $link[ password ];
										$account->data[ address ]       = $link[ address ];
										$account->data[ city ]          = $link[ city ];
										$account->data[ state ]         = $link[ state ];
										$account->data[ zip ]           = $link[ zip ];
										$account->data[ phone ]         = $link[ phone ];
										$account->data[ fax ]           = $link[ fax ];
										$account->data[ country ]       = $link[ country ];
										if ( $link[ name ] ) {
												$account->data[ contact_name ] = $link[ name ];
										} else {
												$account->data[ contact_name ] = $link[ login ];
										}
										$account->data[ contact_email ]   = $link[ email ];
										$account->data[ current_balance ] = $config[ jackhammer_default_balance ];
										$account->update_values ( $account->data );
										$account->save ();
										// if( $CGI[email] ) {
										if ( $CGI[ email ] ) {
												$template = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $template_code , "values" => $link ) );
												$body     = $template->merge ();
												mail ( $link[ email ] , $CGI[ email ] , $body , "From: $config[email]\r\n" );
												$report .= "$language[accounts_mail_sent]<br />\n";
										}
										$report .= "<br />\n";
								}
								$output = "
                  $language[accounts_created_header]<br /><br />
                  $language[accounts_total] $total<br />
                  $language[accounts_unassigned] $unassigned
                  <br /><br />
                  $report
              ";
						} else {
								$email_options = '';
								$SQL           = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'EMAIL'";
								$letters       = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $letters as $email ) {
										$email_options .= " <option value='$email[template_name]'>$email[template_name]</option> ";
								}
								$categories = category_option_list ();
								$output     = "
                  $language[accounts_total] $total<br />
                  $language[accounts_unassigned] $unassigned
                  <br /><br />
                  <form action = '$SCRIPT_NAME' method='post'>
                      <b>$language[accounts_create_header]</b><br />
                      $language[accounts_select_category]<br />
                      <select name='categories[]' multiple size=5>
                      $categories
                      </select>
                      <br /><br />
                      $language[accounts_send_email]
                      <select name='email'>
                         <option value=''>No</option>
                         $email_options
                      </select><br />

                      <input type='hidden' name='action' value='user_accounts'>
                      <input type='hidden' name='admin' value='hyperseek'>
                      <input type='hidden' name='create_accounts' value='1'>
                      <input type='submit' value='$language[accounts_create_button]'>
                  </form>
              ";
						}
						print admin_output ( $language[ accounts_title ] , $output );
				}


				#--------------------------------------------------------------------------#
				#  Validate each link in the database via HTTP                             #
				#--------------------------------------------------------------------------#
				function validate ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $templates;
						$batch = $GLOBALS[ argv ][ 1 ] || 100;
						$count = 0;
						echo $language[ verify_title ];
						if ( $batch ) {
								echo " $language[verify_batch] $batch .... \n";
						} else {
								echo " .... \n";
						};
						$SQL           = "SELECT id FROM " . VALIDATE_TABLE;
						$last_verified = $iSQL->search ( array ( "SQL" => $SQL , "Return" => 'single' ) );
						$SQL           = "DELETE FROM " . VALIDATE_TABLE;
						$iSQL->run ( $SQL );
						$SQL = "INSERT INTO " . VALIDATE_TABLE . " VALUES(0)";
						$iSQL->run ( $SQL );
						$SQL   = "SELECT id, url FROM " . LINKS_TABLE;
						$links = $iSQL->search ( array ( "SQL" => $SQL ) );
						$ok    = 0;
						if ( !$last_verified ) {
								$ok = 1;
						}
						foreach ( $links as $listing ) {
								if ( $listing[ id ] == $last_verified ) {
										$ok = 1;
								}
								if ( $ok != 1 ) {
										continue;
								}
								$urlparts = parse_url ( $listing[ url ] );
								$hostname = $urlparts[ host ];
								$file     = $urlparts[ path ];
								if ( !$file ) {
										$file = '/';
								}
								$fp = fsockopen ( $hostname , 80 , $errno , $errstr , 5 );
								if ( $fp ) {
										$sstatus = "OK";
										fputs ( $fp , "GET $file HTTP/1.0\n\n" );
										for ( $try = 1; $try <= 3; $try++ ) {
												$fstatus = "CHECKING";
												if ( ( $got = fgets ( $fp , 256 ) ) == NULL ) {
														break;
												}
										}
										preg_match ( "/HTTP\/1.(.) (.*) (.*)/i" , $got , $parts );
										$code = $parts[ 2 ];
								} else {
										$code = "700";
								}
								$contents = file ( $listing[ url ] );
								if ( $contents ) {
										print "+";
								}
								if ( $code == 200 ) {
										print "+";
								} elseif ( $code == 600 ) {
										print "#";
								} elseif ( $code == 700 ) {
										print "*";
								} else {
										print "-";
								}
								echo " $listing{url} : [$code] $message \n";
								$report                      = new Record ( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Lookup" => 1 , "Schema" => LINKS_TABLE , "id" => $listing[ id ] ) );
								$report->data[ last_status ] = $code;
								$report->update_values ();
								$report->save ();
								$SQL = "UPDATE " . VALIDATE_TABLE . " SET id = $listing[id]";
								$iSQL->run ( $SQL );
								$count++;
								if ( $count >= $batch ) {
										break;
								}
						}
						if ( $count < $batch ) {
								print $language[ verify_done ] . "\n";
						} else {
								print $language[ verify_batch_done ] . "\n";
						}
						exit;
				}


				#--------------------------------------------------------------------------#
				#  Report the validation results for the listing URLs in the database      #
				#--------------------------------------------------------------------------#
				function view_verify_report ()
				{
						global $CGI;
						global $oSQL;
						global $language;
						global $config;
						$list  = '';
						$SQL   = "SELECT * FROM " . LINKS_TABLE . " WHERE last_status NOT IN('','200')";
						$links = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $links as $link ) {
								$list .= "
                  <input type='checkbox' name='delete[]' value='$link[id]'>Delete ...
                  [ <a href='javascript:OpenWin(\"$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$link[id]\");'>Edit</a> ] ...
                  Failure for <a href='$link[url]' target='editlink'>$link[title]</a>: ($link[last_status])<br />
                ";
						}
						$emails  = '';
						$SQL     = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'EMAIL'";
						$letters = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $letters as $email ) {
								$emails .= " <option value='$email[template_name]'>$email[template_name]</option> ";
						}
						if ( $list ) {
								$OUTPUT = "
                   <script language='javascript'>
                      function OpenWin(Loc) {
                        X=window.open(Loc,'xyz','toolbar=no,scrollbars=yes,status=yes,directories=no,resizable=yes,menubar=no,width=625,height=500')
                      }

                      function QuickCheck(c_how) {
                          if(c_how == 0) {
                             for (var i=0;i<document.submit_form.elements.length;i++) {
                                var e=document.submit_form.elements[i];
                                e.checked = false;
                             }
                          }
                          if(c_how == 1) {
                             for (var i=0;i<document.submit_form.elements.length;i++) {
                                var e=document.submit_form.elements[i];
                                var n = e.name;
                                if ( n.indexOf('delete') >= 0 ) { e.checked = true; }
                                else { e.checked = false; }
                             }
                          }
                      }
                   </script>

                   <table border=0 width=500>
                   <tr><td>
                        $language[verify_explanation]
                        <form name='submit_form' action='$script_name' method='post'>
                          $language[verify_email_prompt]
                             <select name='email'>
                                 <option value=''>don't send</option>
                                 $emails
                             </select>
                          <br />
                          <input type='checkbox' name='screen' value='yes'>$language[verify_screen_prompt]
                          <input type='button' value='delete all' onclick='QuickCheck(1);'>
                          <input type='button' value='uncheck all' onclick='QuickCheck(0);'>
                          <input type='hidden' name='action' value='process_dead_links'>
                          <input type='hidden' name='admin' value='hyperseek'>
                          <input type='submit' value='Process'>
                          <hr />
                          <p>
                          $list
                       </form>
                    </td></tr>
                    </table>
                 ";
						} else {
								$OUTPUT = $language[ verify_report_not_run ];
						}
						admin_output ( $language[ verify_title ] , $OUTPUT );
				}


				#--------------------------------------------------------------------------#
				#  Mass Delete / Email dead listings                                       #
				#--------------------------------------------------------------------------#
				function process_dead_links ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						global $SCRIPT_NAME;
						global $templates;
						$ids = $CGI[ 'delete' ];
						if ( $CGI[ 'email' ] ) {
								$template_code = $templates[ 'hs_admin' ][ 'default' ][ 'EMAIL' ][ $CGI[ 'email' ] ];
						}
						foreach ( $ids as $id ) {
								$record  = new Listing( array ( "Parent" => $iSQL , "Table" => LINKS_TABLE , "Schema" => LINKS_TABLE , "Lookup" => 1 , "id" => $id ) );
								$message = $record->delete ();
								$title   = $record->data[ title ];
								$email   = $record->data[ email ];
								echo "Deleted: $title ($message) ... ";
								if ( $CGI[ email ] ) {
										$template = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $template_code , "values" => $record->data ) );
										$body     = $template->merge ();
										mail ( $email , $CGI[ email ] , $body , "From: $config[email]\r\n" );
										$output .= "Sent email to $email";
								}
								echo $output . "<br />\n";
						}
				}


				#--------------------------------------------------------------------------#
				#  Send Emails to webmasters / Optin lists                                 #
				#--------------------------------------------------------------------------#
				function webmaster_mailing ()
				{
						global $SCRIPT_NAME;
						global $language;
						global $CGI;
						$category_options = '';
						$catlink          = '';
						if ( $CGI[ listcats ] == 1 ) {
								$category_options = category_option_list ();
						} else {
								$catlink = " ( <a href='$SCRIPT_NAME?admin=hyperseek&action=webmaster_mailing&listcats=1'>List Categories</a> ) ";
						}
						$OUTPUT = "
              <table border='0' cellspacing='20' width='400'>
               <form action='$SCRIPT_NAME' method='post'>
               <tr>
                <td valign='top'>
                   $language[webmaster_mailing_select]<br />
                   <select name='webmasters'>
                      <option value='all'>$language[webmaster_mailing_all]</option>
                      <option value='jackhammer'>$language[webmaster_mailing_jackhammer]</option>
                      <option value='affiliates'>$language[webmaster_mailing_affiliates]</option>
                      <option value='moderators'>$language[webmaster_mailing_moderators]</option>
                      $category_options
                   </select> $catlink
                   <br /><br />
                   $language[webmaster_mailing_email_prompt]<br /><br />
                   Subject: <input name='subject'><br /><br />
                   Email Text: <br />
                   <textarea name='email_text' cols=60 rows=8></textarea>
                   <br /><br />
                   <center>
                      <input type='hidden' name='action' value='send_webmaster_mailing'>
                      <input type='hidden' name='admin' value='hyperseek'>
                      <input type='submit' value='$language[webmaster_mailing_button]'>
                   </center>
                </td>
               </tr>
               </form>
              </table>
           ";
						admin_output ( $language[ webmaster_mailing_title ] , $OUTPUT );
				}

				#--------------------------------------------------------------------------#
				#  Send an email to all/selected webmasters                                #
				#--------------------------------------------------------------------------#
				function send_webmaster_mailing ()
				{
						global $CGI;
						global $oSQL;
						global $templates;
						global $config;
						// Table Vars
						$links_table      = LINKS_TABLE;
						$accounts_table   = ACCOUNTS_TABLE;
						$affiliate_table  = AFF_INFO_TABLE;
						$moderators_table = MODERATORS_TABLE;
						if ( $CGI[ webmasters ] == "all" ) {
								$SQL = " SELECT * FROM $links_table ";
						} elseif ( $CGI[ webmasters ] == "jackhammer" ) {
								$SQL = "
                    SELECT $links_table.*, $accounts_table.*
                    FROM $links_table, $accounts_table
                    WHERE $links_table.account = $accounts_table.acct_id
                ";
						} elseif ( $CGI[ webmasters ] == "affiliates" ) {
								$SQL = "
                    SELECT $affiliate_table.*, $accounts_table.*
                    FROM $affiliate_table, $accounts_table
                    WHERE $affiliate_table.affiliate = $accounts_table.acct_id
                ";
						} elseif ( $CGI[ webmasters ] == "moderators" ) {
								$SQL = "
                    SELECT $moderators_table.*, $accounts_table.*
                    FROM $moderators_table, $accounts_table
                    WHERE $moderators_table.moderator_id = $accounts_table.acct_id
                ";
						} else {
								$query = " SELECT id FROM " . CATS_TABLE . " WHERE category = '$CGI[webmasters]' ";
								$ids   = $oSQL->search ( array ( "SQL" => $query , "Return" => "array" ) );
								$in    = implode ( "," , $ids );
								if ( $in ) {
										$SQL = "
                        SELECT $links_table.*
                        FROM $links_table
                        WHERE $links_table.id in($in)
                    ";
								}
						}
						if ( $SQL && $CGI[ webmasters ] ) {
								$results = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "set" ) );
						}
						echo "TO: $CGI[webmasters]<br />\n";
						echo "SQL: $SQL<br /><br />";
						$list   = array ();
						$unique = array ();
						foreach ( $results as $listing ) {
								if ( $listing[ acct_id ] ) {
										$acct = 1;
								} elseif ( $listing[ email ] ) {
										$listing[ contact_email ] = $listing[ email ];
								}
								if ( $listing[ contact_email ] ) {
										if ( !$unique[ $listing[ contact_email ] ] ) {
												$template                            = new SubTemplate( array ( "front" => "[" , "back" => "]" , "template_text" => $CGI[ email_text ] , "values" => $listing ) );
												$body                                = $template->merge ();
												$unique[ $listing[ contact_email ] ] = 1;
												array_push ( $list , array ( $listing[ contact_email ] , $body ) );
										}
								}
						}
						foreach ( $list as $email ) {
								$to      = $email[ 0 ];
								$message = $email[ 1 ];
								$from    = 'eClickZ.com';
								// To send HTML mail, the Content-type header must be set
								$headers = 'MIME-Version: 1.0' . "\r\n";
								$headers .= 'Content-type: text/html; charset=iso-8859-1' . "\r\n";
								// Additional headers
								$headers .= 'From: eClickZ.com <info@eclickz.com>' . "\r\n";
								mail ( $to , $CGI[ subject ] , $message , $headers );
								echo "$language[webmaster_mailing_sent] TO: $to<br />\n\n";
						}
						echo "</pre>";
				}

				#--------------------------------------------------------------------------#
				#  Re Initialize the statistics                                            #
				#--------------------------------------------------------------------------#
				function init_top10 ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						$output = '';
						if ( $CGI[ confirm ] ) {
								$SQL = "UPDATE " . LINKS_TABLE . " SET top_votes = 0";
								if ( $iSQL->run ( $SQL ) ) {
										$output = $language[ init_top10_ok ];
								} else {
										$output = $language[ init_top10_error ];
								}
						} else {
								$output = "
                 <br />
                 $language[init_top10_confirm]
                 <br /><br />
                 <a href='$SCRIPT_NAME?admin=hyperseek&action=init_top10&confirm=1'>$language[init_top10_confirm_link]</a>
              ";
						}
						print admin_output ( $language[ init_top10_title ] , $output );
				}

				function init_ratings ()
				{
						global $iSQL;
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						$output = '';
						if ( $CGI[ confirm ] ) {
								$SQL = "UPDATE " . LINKS_TABLE . " SET rate_votes = 0, rate_score = 0";
								if ( $iSQL->run ( $SQL ) ) {
										$output = $language[ init_top10_ok ];
								} else {
										$output = $language[ init_top10_error ];
								}
						} else {
								$output = "
                 <br />
                 $language[init_rating_confirm]
                 <br /><br />
                 <a href='$SCRIPT_NAME?admin=hyperseek&action=init_ratings&confirm=1'>$language[init_rating_confirm_link]</a>
              ";
						}
						print admin_output ( $language[ init_rating_title ] , $output );
				}


				#--------------------------------------------------------------------------#
				#  Regenerate the link counts for each category                            #
				#--------------------------------------------------------------------------#
				function category_count ()
				{
						global $CGI;
						global $iSQL;
						global $SCRIPT_NAME;
						global $config;
						global $language;
						global $argv;
						$message    = '';
						$catcount   = array ();
						$newcount   = array ();
						$catparents = array ();
						$catkids    = array ();
						$names      = array ();
						$total      = 0;
						// Table Vars
						$links_table = LINKS_TABLE;
						$cats_table  = CATS_TABLE;
						echo "<span style='border:1px ridge #e0e0e0; background:#efefef; width:40px; text-align:center'>ID</span>\n";
						echo "<span style='border:1px ridge #e0e0e0; background:#efefef; width:40px; text-align:center'>Count</span>\n";
						echo "<span style='border:1px ridge #e0e0e0; background:#efefef; text-align:center'>Category Name</span>\n";
						echo "<br />";
						// Get List of all Category Names and IDs
						$SQL     = "SELECT category_id, category_name FROM " . CATEGORY_TABLE . " ORDER BY parent_id ASC, category_id ASC";
						$results = $iSQL->search ( array ( "SQL" => $SQL ) );
						// Get the raw count of every category, ensuring only active, valid links are included.
						foreach ( $results as $category ) {
								$cid           = $category[ category_id ];
								$name          = $category[ category_name ];
								$names[ $cid ] = $name;
								$SQL           = "SELECT count(*) as ccount FROM $cats_table, $links_table WHERE $cats_table.category = '$cid' AND $cats_table.id = $links_table.id";
								$counts        = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $counts as $cat ) {
										$catcount[ $cid ] = $cat[ ccount ];
										$newcount[ $cid ] = 0;
										$total += $cat[ ccount ];
								}
								// GET DEMOGRAPHICS HERE ??
								$obj = new CategoryNavigator ( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $cid ) );
								// list ($parents, $children) = $obj->demographics();
								$children           = $obj->children ( array ( "idsonly" => 1 ) );
								$parents            = $obj->parents ( array ( "idsonly" => 1 ) );
								$catparents[ $cid ] = $parents;
								$catkids[ $cid ]    = $children;
						}
						krsort ( $catcount );
						foreach ( $catcount as $cat => $count ) {
								$children = $catkids[ $cat ];
								## Child Counts
								foreach ( $children as $cid => $child ) {
										$cnt = $catcount[ $child ];
										$newcount[ $child ] += $cnt;
										$newcount[ $cat ] += $newcount[ $child ];
										$cname = $names[ $child ];
								}
						}
						ksort ( $names );
						foreach ( $names as $cat => $name ) {
								$cp = array ();
								foreach ( $catparents[ $cat ] as $pname ) {
										array_push ( $cp , $names[ $pname ] );
								}
								$fullname = implode ( ':' , array_reverse ( $cp ) );
								if ( $fullname ) {
										$fullname .= ":$name";
								} else {
										$fullname = $name;
								}
								$plist    = implode ( ',' , $catparents[ $cat ] );
								$clist    = implode ( ',' , $catkids[ $cat ] );
								$savename = preg_replace ( "/\'/" , "''" , $fullname );
								// $newcount[$cat] += $catcount[$cat];
								$SQL = "UPDATE " . CATEGORY_TABLE . " SET cat_count = $newcount[$cat], full_name = '$savename' WHERE category_id = '$cat'";
								$iSQL->run ( $SQL );
								$ctree                      = new Record( array ( "Parent" => $iSQL , "Table" => TREE_TABLE , "Schema" => TREE_TABLE , "Lookup" => 1 , "category_id" => $cat ) );
								$ctree->data[ category_id ] = $cat;
								$ctree->data[ parents ]     = $plist;
								$ctree->data[ children ]    = $clist;
								$ctree->update_values ( $ctree->data );
								$ctree->save ();
								echo "<span style='width:40px;text-align:right'>$cat</span>\n";
								echo "<span style='width:40px;text-align:right'>$newcount[$cat]</span>\n";
								echo "<span style='padding-left:10px'>$fullname</span>\n";
								echo "<br />";
						}
						echo "<br /><br /><H3>$language[category_count_final] ($total)</H3>\n";
						echo "</div>";
						exit;
				}


				#--------------------------------------------------------------------------#
				#  Ensure that every link is assigned to a properly defined category       #
				#--------------------------------------------------------------------------#
				function category_validate ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $config;
						global $language;
						$invalid     = '';
						$num_invalid = 0;
						$empty       = '';
						$num_empty   = 0;
						$checked     = 0;
						$bad         = 0;
						## First: A list of all valid category ID's
						$SQL            = "SELECT category_id, 1 FROM " . CATEGORY_TABLE . " ORDER BY category_id";
						$valid_category = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'fullhash' ) );
						## Now, go through the links, find out what they've been assigned to
						## Note, we're going to program the DBI directly here, rather than use objects
						##       just in case they've got a zillion of them, and memory would be a problem.
						$SQL   = "SELECT id, category, title FROM " . LINKS_TABLE;
						$links = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $links as $link ) {
								$id          = $link[ id ];
								$assignedcat = $link[ category ];
								$title       = $link[ title ];
								$checked++;
								foreach ( split ( ';' , $assignedcat ) as $category ) {
										if ( !$category ) {
												if ( $CGI[ screen ] != "empty" ) {
														$bad++;
														$num_empty++;
														$empty .= "
                           <li>
                                <font color='red'>$language[validate_category_not_assigned]</font> $title ...
                                &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$id'>$language[validate_category_delete]</a>
                                &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$id'>$language[validate_category_edit]</a><p>
                           </li>
                        ";
												}
										} else {
												$cats = split ( ':' , $category );
												foreach ( $cats as $cx ) {
														if ( !$cx ) {
																continue;
														}
														if ( !$valid_category[ $cx ] ) {
																$bad++;
																$num_invalid++;
																$invalid .= "
                             <li>
                                  <font color='green'>$language[validate_category_invalid] ($cx)</font> $title ...
                                  &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=delete_link&id=$id'>$language[validate_category_delete]</a>
                                  &#187; <a href='$SCRIPT_NAME?admin=hyperseek&action=edit_link&id=$id'>$language[validate_category_edit]</a><p>
                             </li>
                          ";
														}
												}
										}
								}
						}
						if ( !$checked ) {
								$checked = 0;
						}
						if ( !$bad ) {
								$bad = 0;
						}
						$OUTPUT = "
              <table>
               <tr>
                <td colspan='2'>
                   <b>$language[validate_category_total]</b> $checked <br />
                   <b>$language[validate_category_invalid_total]</b> $bad  (Empty: $num_empty / Invalid: $num_invalid)
                   <ul>
                   $empty
                   $invalid
                   </ul>
                </td>
               </tr>
              </table>
           ";
						admin_output ( $language[ validate_category_title ] , $OUTPUT );
				}

				function pick_account ()
				{
						ini_set ( 'memory_limit' , '1024M' );
						global $CGI;
						global $language;
						global $oSQL;
						$accounts = '';
						$type     = '';
						if ( $CGI[ 'type' ] == "affiliate_id" ) {
								$type    = "Affiliate";
								$SQL     = "SELECT affiliate FROM " . AFF_INFO_TABLE . " ORDER BY affiliate";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $results as $account ) {
										$accounts .= " <option value='" . htmlspecialchars ( $account[ affiliate ] ) . "'>" . htmlspecialchars ( $account[ affiliate ] ) . "</option> ";
								}
						} elseif ( $CGI[ 'type' ] == "source" ) {
								$type    = "Source";
								$SQL     = "SELECT engine_name FROM " . XML_FEEDS_TABLE . " ORDER BY engine_name";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $results as $account ) {
										$accounts .= " <option value='$account[engine_name]'>$account[engine_name]</option> ";
								}
						} else {
								$type    = "Account";
								$SQL     = "SELECT acct_id,contact_name FROM " . ACCOUNTS_TABLE . " ORDER BY acct_id";
								$results = $oSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $results as $account ) {
										$accounts .= " <option value='" . htmlspecialchars ( $account[ acct_id ] ) . "'>" . htmlspecialchars ( $account[ acct_id ] . "(" . $account[ contact_name ] . ")" ) . "</option> ";
								}
						}
						echo "
              <html>
              <head>
                 <title>Pick Filter</title>
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
                  Select $type
                    <select name='account'>
                      <option value=''>ALL</option>
                      $accounts
                    </select>
                    <br /><br />
                    <input type='button' value='Use Selected' onClick='UpdateParent(this.form)'>
                </form>
              </center>
           ";
						exit;
				}
		} // End Class

		// Functions that must execute outside the class definition //

		#--------------------------------------------------------------------------#
		#  List and Edit  Detail (link, category) templates                        #
		#--------------------------------------------------------------------------#

		function listing_template_list ()
		{
				global $currentlink;
				global $oSQL;
				$templ     = $currentlink->data[ extra_link ];
				$list      = " <option value='$templ'>$templ</option> ";
				$SQL       = "SELECT * FROM " . TEMPLATES_TABLE . " WHERE program = 'hs_admin' AND theme='default' AND template_type = 'LISTING'";
				$templates = $oSQL->search ( array ( "SQL" => $SQL ) );
				foreach ( $templates as $template ) {
						$list .= " <option value='$template[template_name]'>$template[template_name]</option> ";
				}

				return $list;
		}


		#--------------------------------------------------------------------------#
		#  Select list of qualifiers                                               #
		#--------------------------------------------------------------------------#
		function qualifier_list ()
		{
				global $oSQL;
				global $data_dir;
				global $currentlink;
				$qdat = array ();
				if ( $currentlink ) {
						$cats = split ( ";" , $currentlink->data[ category ] );
						foreach ( $cats as $category ) {
								$cat = new Record ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $category ) );
								if ( $cat->data[ qualifiers ] ) {
										array_push ( $qdat , $cat->data[ qualifiers ] );
								}
						}
				}
				$qfile = "$data_dir/hyperseek/qualifiers.dat";
				if ( file_exists ( $qfile ) ) {
						foreach ( file ( $qfile ) as $q ) {
								array_push ( $qdat , $q );
						}
				}
				if ( $currentlink ) {
						$q = $currentlink->data[ qualifier ];
				}
				$qlist = " <option value='$q'>$q</option> ";
				foreach ( $qdat as $qualifier ) {
						$qualifier = preg_replace ( "/\r|\n|\cM/" , "" , $qualifier );
						$qlist .= " <option value='$qualifier'>$qualifier</option> ";
				}

				return ( $qlist );
		}


		// Quick Queue Viewer
		function queue_window ( $return = 0 )
		{
				global $oSQL;
				global $SCRIPT_NAME;
				global $language;
				global $ADMIN_TYPE;
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'NEW'";
				$submissions = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'CHANGE'";
				$changes     = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'SPIDERED'";
				$spidered    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . DEPOSITS_TABLE;
				$deposits    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . PENDING_BIDS_TABLE;
				$pendingbids = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name LIKE 'Pending%'";
				$affiliates  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . WITHDRAWLS_TABLE;
				$withdrawls  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . TICKETS_TABLE . " WHERE ticket_status = 'O'";
				$opentickets = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$queue       = '';
				if ( $submissions && ( preg_match ( "/administrator|content/" , $ADMIN_TYPE ) ) ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=NEW'>$language[queue_new_link]</a> ($submissions)
               <br />
            ";
				}
				if ( $changes && ( preg_match ( "/administrator|content/" , $ADMIN_TYPE ) ) ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=CHANGE'>$language[queue_changed_link]</a> ($changes)
               <br />
            ";
				}
				if ( $spidered && ( preg_match ( "/administrator|content/" , $ADMIN_TYPE ) ) ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=hyperseek&action=submissions&type=SPIDERED'>$language[queue_new_link]</a> ($spidered)
               <br />
            ";
				}
				if ( $deposits && $ADMIN_TYPE == "administrator" ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=accounts&action=deposits'>$language[queue_pending_deposits]</a> ($deposits)
               <br />
            ";
				}
				if ( $withdrawls && $ADMIN_TYPE == "administrator" ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=accounts&action=withdrawls'>$language[queue_pending_withdrawls]</a> ($withdrawls)
               <br />
            ";
				}
				if ( $opentickets ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=accounts&action=list_tickets&type=O'>$language[open_tickets_link]</a> ($opentickets)
               <br />
            ";
				}
				if ( $pendingbids && $ADMIN_TYPE == "administrator" ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=hyperseek&action=pending_bids'>$language[queue_pending_bids]</a> ($pendingbids)
               <br />
            ";
				}
				if ( $affiliates && ( preg_match ( "/administrator|affiliate/" , $ADMIN_TYPE ) ) ) {
						$queue .= "
               <a style='color:black' href='$SCRIPT_NAME?admin=accounts&action=list_pending_affiliates'>$language[queue_pending_affiliates]</a> ($affiliates)
               <br />
            ";
				}
				$output = "
          <fieldset style='padding:5px;font-family:verdana,arial; font-size:9pt;'>
            <legend style='font-size:12px; font-weight:bold;'>$language[tab_queue]</legend>
            $queue
          </fieldset>
        ";
				if ( $return ) {
						return $output;
				}
				echo "
           <html>
              <head>
                 <title>Queue</title>
                 <meta http-equiv='refresh' content='180'>
              </head>
              <body style='background:#e0e0e0; border:1px ridge silver; margin-top: 3px; overflow:hidden;background:#e0e0e0;'>
                  $output
              </body>
           </html>

        ";
				exit;
		}

?>
