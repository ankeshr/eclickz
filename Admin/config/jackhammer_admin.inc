<?php

		/*
				________________________________________________________________________________

						  PROGRAM : ASP : Admin : Sponsored Links Admin Center (PHP)
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
		$PROGRAMS[ 'jackhammer_admin' ] = "designer,content";
		function config_jackhammer_admin ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $menuitems;
				global $page_templates;
				global $NEW;
				global $CHG;
				global $SPD;
				global $DEP;
				global $BIDS;
				global $AFF;
				global $TTL;
				global $WITH;
				global $SCRIPT_NAME;
				global $ADMIN_TYPE;
				//echo "in" ; 
				$SQL                                                                      = "SELECT template_name FROM " . TEMPLATES_TABLE . " WHERE program='jackhammer_admin' AND theme='default' AND template_type = 'EMAIL'";
				$titles                                                                   = $oSQL->search ( array ( "SQL" => $SQL , "Return" => 'array' ) );
				$opts                                                                     = join ( "|" , $titles );
				$vars[ 'URLS' ][ '18:jackhammer_thankyou_url' ]                           = array ( "The URL for your Thank You page (after an order has been processed)" , "TEXT" , "/search/thanks.html" , "design" );
				$vars[ 'URLS' ][ '19:jackhammer_failure_url' ]                            = array ( "The URL for your Failure page (after an order has been improperly processed)" , "TEXT" , "/search/failure.html" , "design" );
				$vars[ 'jackhammer_admin' ][ '05:jackhammer_amounts' ]                    = array ( "Enter the deposit amounts to show in the select list for the user(use dollars and cents, separate them with commas)" , "TEXT" , "10.00,20.00,30.00" );
				$vars[ 'jackhammer_admin' ][ '06:jackhammer_tax' ]                        = array ( "Enter the tax rate (ie: 8.25\%) to append to deposits that indicate a need to pay taxes (leave blank if no taxes to be collected)" , "TEXT" , "" );
				$vars[ 'jackhammer_admin' ][ '07:jackhammer_deposit_match' ]              = array ( "<I>OPTIONAL</I> Enter the amount (in <B>dd.cc</B> format for money, or <B>nn.nn%</B> for a percent) that you would like to add to every deposit as a matching amount." , "TEXT" , "" );
				$vars[ 'jackhammer_admin' ][ '07:jackhammer_default_balance' ]            = array ( "Default amount to start off all new Jackhammer User with" , "TEXT" , "0.00" );
				$vars[ 'jackhammer_admin' ][ '08:jackhammer_no_price' ]                   = array ( "Eliminate non-paid terms from all search results?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '08:jh_split_paid_results' ]                 = array ( "Split Paid and Free Results into Separate Output Plugins?<br />(if this is set to 'NO', the result set will be 'merged' into a cohesive list)" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '09:jackhammer_addtl_keys' ]                 = array ( "Allow users to add new keywords?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '10:jackhammer_approve_keys' ]               = array ( "Do you want to manually approve all bidded terms?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '11:yh_how_to' ]                             = array ( "Which fields should the search engine automatically parse for pre-generating a keyword bidding list" , "MULTIPLE" , "TITLE|CAT|DESC|KEYS|KEYWORD PHRASES ONLY|INDEX NOTHING" , "content" );
				$vars[ 'jackhammer_admin' ][ '12' ]                                       = array ( "Zones & GeoCoding" , "HEADER" , "" );
				$vars[ 'jackhammer_admin' ][ '13:jackhammer_lexical_searching' ]          = array ( "Allow Lexical(Synonym/plural/soundslike) bidding & searching?<BR>Requires the <a href='http://search.cpan.org/search?dist=Text-Metaphone' target=_blank>Text::Metaphone</A> Module" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '14:bidding_zones' ]                         = array ( "A list of zones, comma/colon separated (Zone:Zone Name) that you'll allow bidding for.  For country specific zones, you must use the 2 letter GeoIP country code<br /><br />For time specific zones, you must use the TZ# code that you create in the options below.<br /><br />(do not use the 'enter key' to make multiple lines)" , "TEXTAREA" , "" , "content" );
				$vars[ 'jackhammer_admin' ][ '15:default_zone' ]                          = array ( "Which of the zones (above) is the default ?" , "TEXT" , "" , "content" );
				$vars[ 'jackhammer_admin' ][ '16:enable_geoip' ]                          = array ( "Enable GeoTargeting ?" , "SELECT" , "NO|YES" , "content" );
				$vars[ 'jackhammer_admin' ][ '17:geoip_zone_assignments' ]                = array ( "If you have enabled both Zones and GeoIP, list here, in comma/colon (countrycode:zone) the country codes you allow, and their associated bidding zone ID<br /><br />(do not use the 'enter key' to make multiple lines)" , "TEXTAREA" , "" , "content" );
				$vars[ 'jackhammer_admin' ][ '18:geoip_zone_unassigned' ]                 = array ( "What do you want to do with searches coming from a country code that you do not have defined above ?" , "SELECT" , "Ignore Search|Place Visitor in the Default Zone" , "content" );
				$vars[ 'jackhammer_admin' ][ '19:enable_time_targeting' ]                 = array ( "Enable Time of Day Targeting ?" , "SELECT" , "NO|YES" , "content" );
				$vars[ 'jackhammer_admin' ][ '19a:time_targets' ]                         = array ( "List your time of day targets, one per line (TZ#:GST-Start-Hour:GST-End-Hour ... ie: TZ1:12:15)" , "TEXTAREA" , "This\nThat\nThe other thing" , "content" );
				$vars[ 'jackhammer_admin' ][ '20' ]                                       = array ( "Bidding Options" , "HEADER" , "" );
				$vars[ 'jackhammer_admin' ][ '21:jackhammer_bidding_format' ]             = array ( "Bidding Type" , "SELECT" , "Default (one to many)|Overture (one to one)" , "content" );
				$vars[ 'jackhammer_admin' ][ '22:jackhammer_hot_zone_page_bidding' ]      = array ( "Do you want to enable 'Hot Zone' keywords in addition to normal search terms?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '23:hot_zone_default_term' ]                 = array ( "If using hotzones, list your hotzone terms/phrases here, separated by semicolons (;)" , "TEXT" , "" , "content" );
				$vars[ 'jackhammer_admin' ][ '24:jackhammer_hot_zone_smartbid_exclude' ]  = array ( "<B>Exclude</B> HotZone Terms from Smartbidding?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '25:jackhammer_hot_zone_smartbid_minimums' ] = array ( "Enforce minimum bids for HotZone Terms?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '26:jackhammer_allow_delete' ]               = array ( "Allow users to delete keywords from their bid screen" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'jackhammer_admin' ][ '27:jackhammer_alert_balance' ]              = array ( "Amount to email account holders when their balance is low" , "TEXT" , "10.00" );
				$vars[ 'jackhammer_admin' ][ '28:jackhammer_bids_per_page' ]              = array ( "For the users, how many bidded terms to show on each page" , "TEXT" , "5" );
				$vars[ 'jackhammer_admin' ][ '30:jackhammer_charge_for' ]                 = array ( "What are you accepting bids for?" , "SELECT" , "CLICKS|DETAIL|MONTHLY LISTING|MONTHLY KEYWORDS" );
				$vars[ 'jackhammer_admin' ][ '31:jackhammer_keyword_bump_limit' ]         = array ( "If using MONTHLY KEYWORDS billing, at which incremental # of hits should the minimum keyword bid level be raised?" , "TEXT" , "100" );
				$vars[ 'jackhammer_admin' ][ '32:jackhammer_keyword_bump' ]               = array ( "If using MONTHLY KEYWORDS billing, how much should the minimum be bumped up for each $config[jackhammer_keyword_bump_limit] clicks the previous month?" , "TEXT" , "0.00" );
				$vars[ 'jackhammer_admin' ][ '33:jackhammer_minimum_bid' ]                = array ( "What is your minimum bid amount (this will be assigned to all new keywords)?" , "TEXT" , "0.00" );
				/*This line added by Akhilesh on 5/3/2011 for add option to set minimum bid price for RON Listing*/
				$vars[ 'jackhammer_admin' ][ '33:jackhammer_minimum_bid_ron' ]    = array ( "What is your minimum bid amount for RON listings? (this will be assigned to all new keywords)?" , "TEXT" , "0.00" );
				$vars[ 'jackhammer_admin' ][ '34:jackhammer_bid_increment' ]      = array ( "Bid Increment" , "TEXT" , ".01" );
				$vars[ 'jackhammer_admin' ][ '35:sponsor_box_fee' ]               = array ( "Sponsor Box Fee" , "TEXT" , "50.00" );
				$vars[ 'jackhammer_admin' ][ '36:sponsor_box_time' ]              = array ( "# of days sponsor box ads are active for above fee" , "TEXT" , "30" );
				$vars[ 'jackhammer_admin' ][ '37:sponsor_box_max' ]               = array ( "# of sponsors to allow per term" , "TEXT" , "3" );
				$vars[ 'jackhammer_admin' ][ '38:low_balance_email' ]             = array ( "Name of email (From internal templates: Jackhammer) to send when accounts reach <B>$config[jackhammer_alert_balance]</B>" , "SELECT" , $opts );
				$vars[ 'jackhammer_admin' ][ '39:no_balance_email' ]              = array ( "Name of email (From internal templates: Jackhammer) to send when accounts reach <B>0.00</B>" , "SELECT" , $opts );
				$vars[ 'PPC-Fraud' ][ '70' ]                                      = array ( "Fraud Settings" , "HEADER" , "" , "content" );
				$vars[ 'PPC-Fraud' ][ '71:jackhammer_fraud_banned_ip' ]           = array ( "Check against the above banned IP List" , "SELECT" , "YES|YES -- and Check before search|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '72:jackhammer_fraud_logged_in' ]           = array ( "Disallow logged in users from clicking links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '73:jackhammer_fraud_link_age' ]            = array ( "Disallow old results (over 5 Minutes) to be clicked" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '74:jackhammer_fraud_unique_link' ]         = array ( "Ensure that each click is completely unique" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '75:jackhammer_fraud_cookie' ]              = array ( "Set/Check click cookies" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '76:jackhammer_fraud_max_hits_for_ip' ]     = array ( "Disallow each IP from clicking more than x times daily" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '77:jackhammer_fraud_link_visited_before' ] = array ( "Disallow multiple clicks on the same url, regardless of search term" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '78:jackhammer_fraud_valid_referer' ]       = array ( "Validate affiliates referring URL" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '80:jackhammer_fraud_search_click_ip' ]     = array ( "Ensure that the IP of the clicker matches that of the searcher" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '81:jackhammer_fraud_positive_balance' ]    = array ( "Ignore clicks when accounts have a negative balance" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '82:jackhammer_fraud_interim_click' ]       = array ( "Force an 'iterim click' on every paid listing" , "SELECT" , "YES|NO" , "content" );




		        $vars[ 'PPC-Fraud' ][ '55:jackhammer_fraud_mask_referring_url' ]       = array ( "Mask Referring URL" , "SELECT" , "YES|NO" , "content" );

				$vars[ 'PPC-Fraud' ][ '83:jackhammer_fraud_user_agent' ]          = array ( "Ensure a valid Browser User Agent" , "SELECT" , "YES|YES -- and Check before search|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '84:jackhammer_fraud_browser_variables' ]   = array ( "Check for valid browser requests (environment variables that browsers send along)" , "SELECT" , "YES|YES -- and Check before search|NO" , "content" );
				$vars[ 'PPC-Fraud' ][ '100' ]                                     = array ( "Advanced & Extra Strict Fraud Checking Options" , "HEADER" , "" , "content" );
				$vars[ 'PPC-Fraud' ][ '101:jackhammer_ignore_ips' ]               = array ( "List IP Numbers, one per line, that require special attention.<BR>Enter IPNUMBER:hits for each one<BR>ie: <B>192.168.0.1:1000</B> would allow 1000 hits for that IP number, and <B>192.168.0.2:ban</B> would ban that ip number." , "TEXTAREA" , "" , "content" );
				$vars[ 'PPC-Fraud' ][ '102:jackhammer_ignore_agents' ]            = array ( "List Browser User Agents Numbers, one per line, to ignore." , "TEXTAREA" , "" , "content" );
				$vars[ 'PPC-Fraud' ][ '103:jackhammer_max_daily_clicks' ]         = array ( "Maximum number of clicks per IP" , "TEXT" , "100" , "content" );
				$vars[ 'templates' ][ LAYOUT ]                                    = "Bid Screen|Each Term|Top 5|SEARCHBOXCODE|Xml List";
				$page_templates[ pages ]                                          = "accounts|account-login|account-create|account-edit|affiliates|affiliates-login|affiliates-create|affiliates-edit";
				## Dynamic Menu Stuff
				if ( $ADMIN_TYPE == "designer" ) {
						return;
				}
				// Affiliate Manager (and admin) links
				if ( preg_match ( "/affiliate|administrator/" , $ADMIN_TYPE ) ) {
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ 'XLS Downloadable Summary' ]         = "$SCRIPT_NAME?admin=hyperseek&action=excel_report_form";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'aff_report_title' ] ]    = "$SCRIPT_NAME?admin=hyperseek&action=affiliate_report";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'click_report' ] ]        = "$SCRIPT_NAME?admin=hyperseek&action=click_report_form";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'trend_report_prompt' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=click_report_form&report_type=trend_report";
						if ( $AFF >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_pending_affiliates] ($AFF)" ] = "$SCRIPT_NAME?admin=accounts&action=list_pending_affiliates";
						}
						$menuitems[ accounts ][ categories ][ $language[ 'tab_affiliates' ] ]                                         = 2;
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_affiliates' ] ][ $language[ 'auditing_link' ] ]          = "$SCRIPT_NAME?admin=hyperseek&action=affiliate_audit";
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_affiliates' ] ][ $language[ 'adjustment_link' ] ]        = "$SCRIPT_NAME?admin=accounts&action=adjust_balance&form=1";
						$menuitems[ 'accounts' ][ 'items' ][ $language[ 'tab_affiliates' ] ][ $language[ 'list_affiliates_prompt' ] ] = "$SCRIPT_NAME?admin=accounts&action=list_affiliates&nl=1";
				}
				// Content Manager (and admin) links
				if ( preg_match ( "/content|administrator/" , $ADMIN_TYPE ) ) {
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_main' ] ][ $language[ 'keyword_minimum_menu' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=keyword_minimums";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_main' ] ][ $language[ 'sponsor_boxes' ] ]        = "$SCRIPT_NAME?admin=hyperseek&action=sponsor_boxes";
						if ( $BIDS >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_pending_bids] ($BIDS)" ] = "$SCRIPT_NAME?admin=hyperseek&action=pending_bids";
						}
				}
				// Admin Only Links
				if ( preg_match ( "/administrator/" , $ADMIN_TYPE ) ) {
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'report_search_stats' ] ]      = "$SCRIPT_NAME?admin=hyperseek&action=search_stats";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'report_keywords' ] ]          = "$SCRIPT_NAME?admin=hyperseek&action=keyword_performance";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'report_categories' ] ]        = "$SCRIPT_NAME?admin=hyperseek&action=keyword_performance&categoriesonly=1";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'report_financial_summary' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=click_report_form&report_type=financial_summary";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_main' ] ][ $language[ 'promotions_link' ] ]                     = "$SCRIPT_NAME?admin=hyperseek&action=promotions";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_main' ] ][ "Merge Tables (Logs)" ]                              = "$SCRIPT_NAME?admin=hyperseek&action=merge_tables";
						if ( $DEP >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_pending_deposits] ($DEP)" ] = "$SCRIPT_NAME?admin=accounts&action=deposits";
						}
						if ( $WITH >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_pending_withdrawls] ($WITH)" ] = "$SCRIPT_NAME?admin=accounts&action=withdrawls";
						}
				}
				// Links for everyone
				$SQL                                                                                                                    = "SELECT COUNT(*) FROM " . TICKETS_TABLE . " WHERE ticket_status = 'O'";
				$OPENTICKETS                                                                                                            = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL                                                                                                                    = "SELECT COUNT(*) FROM " . TICKETS_TABLE . " WHERE ticket_status = 'C'";
				$CLOSEDTICKETS                                                                                                          = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$menuitems[ 'accounts' ][ 'categories' ][ $language[ 'list_tickets' ] ]                                                 = 6;
				$menuitems[ 'accounts' ][ 'items' ][ $language[ 'list_tickets' ] ][ "$language[open_tickets_link] ($OPENTICKETS)" ]     = "$SCRIPT_NAME?admin=accounts&action=list_tickets&type=O";
				$menuitems[ 'accounts' ][ 'items' ][ $language[ 'list_tickets' ] ][ "$language[closed_tickets_link] ($CLOSEDTICKETS)" ] = "$SCRIPT_NAME?admin=accounts&action=list_tickets&type=C";
		}

?>