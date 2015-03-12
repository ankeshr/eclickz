<?php

		/*
		________________________________________________________________________________

						PROGRAM : ASP : Admin : Search Engine Admin Center (PHP)
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
		$PROGRAMS[ 'hs_admin' ] = "designer,content";
		function config_hs_admin ()
		{
				global $oSQL;
				global $config;
				global $language;
				global $vars;
				global $page_templates;
				global $menuitems;
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
				$vars[ 'Administrative' ][ '03:query_string_format' ]                    = array ( "Dynamic URL Format" , "SELECT" , "STANDARD|PATH INFO" , "" );
				$vars[ 'URLS' ][ '14:redirect_url' ]                                     = array ( "The (full) URL to your redirect program" , "TEXT" , "/search/r.php" , "" );
				$vars[ 'URLS' ][ '15:search_url' ]                                       = array ( "The (full) URL to your search program" , "TEXT" , "/search/search.php" , "" );
				$vars[ 'URLS' ][ '16:jackhammer_accounts_url' ]                          = array ( "Full URL (including http://... to your account login program" , "TEXT" , "" , "" );
				$vars[ 'Style' ][ '10' ]                                                 = array ( "Search Engine Specific Style Settings" , "HEADER" , "" , "design" );
				$vars[ 'Style' ][ '11:Engine_Name' ]                                     = array ( "What is the name of your search engine" , "TEXT" , "My Search Engine" , "design" );
				$vars[ 'Style' ][ '12:yh_columns' ]                                      = array ( "Number of category columns on the main page" , "TEXT" , "2" , "design" );
				$vars[ 'Style' ][ '13:yh_main_border' ]                                  = array ( "Border size (0,1,2,etc) of Main Page Category Layout" , "TEXT" , "0" , "design" );
				$vars[ 'Style' ][ '14:yh_hor_spacing' ]                                  = array ( "Main page column spacing (Horizontal)" , "TEXT" , "10" , "design" );
				$vars[ 'Style' ][ '15:yh_ver_spacing' ]                                  = array ( "Main page column spacing (Vertical)" , "TEXT" , "10" , "design" );
				$vars[ 'Style' ][ '16:yh_main_width' ]                                   = array ( "How wide should the main page category listing be" , "TEXT" , "95%" , "design" );
				$vars[ 'Style' ][ '17:yh_menu_columns' ]                                 = array ( "Number of category columns (Results Pages)" , "TEXT" , "3" , "design" );
				$vars[ 'Style' ][ '18:yh_cat_width' ]                                    = array ( "How wide should the category lists be (in result pages)" , "TEXT" , "95%" , "design" );
				$vars[ 'Style' ][ '19:yh_related_columns' ]                              = array ( "For related categories, how many columns to draw" , "TEXT" , "2" , "design" );
				$vars[ 'Style' ][ '20:yh_big_folder' ]                                   = array ( "URL to the <B>big folder</B>" , "TEXT" , "/search/images/big_folder.gif" , "design" );
				$vars[ 'Style' ][ '21:yh_small_folder' ]                                 = array ( "URL to the <b>small folder</B>" , "TEXT" , "/search/images/folder.gif" , "design" );
				$vars[ 'Style' ][ '22:yh_cat_color' ]                                    = array ( "What color should category links be?" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '23:yh_link_color' ]                                   = array ( "Listing Link Color" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '24:yh_cool_icon' ]                                    = array ( "URL for your <B>whats cool</B> icon" , "TEXT" , "/search/images/cool.gif" , "design" );
				$vars[ 'Style' ][ '25:yh_new_icon' ]                                     = array ( "URL for the <B>whats new</b> icon for links 1 day old" , "TEXT" , "/search/images/new.gif" , "design" );
				$vars[ 'Style' ][ '26:yh_new_icon_5' ]                                   = array ( "URL for the <B>whats new</b> icon for links less than 7 days old" , "TEXT" , "/search/images/new5.gif" , "design" );
				$vars[ 'Style' ][ '27:yh_new_icon_10' ]                                  = array ( "URL for the <B>whats new</b> icon for links less than 14 days old" , "TEXT" , "/search/images/new20.gif" , "design" );
				$vars[ 'Style' ][ '28:yh_show_li' ]                                      = array ( "Would you like the listings drawn as <B>dotted,numbered,or plain</B>" , "SELECT" , "LI|OL|Normal" , "design" );
				$vars[ 'Style' ][ '29:yh_color1' ]                                       = array ( "Alternating Color 1,if desired" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '30:yh_color2' ]                                       = array ( "Alternating Color 2,if desired" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '31:yh_max_results' ]                                  = array ( "Max results returned before search warning" , "TEXT" , "1000" );
				$vars[ 'Style' ][ '32:yh_bold' ]                                         = array ( "How do we identify matched terms in results" , "SELECT" , "Do Nothing|BOLD|RED|BLUE|YELLOW HIGHLIGHT" , "design" );
				$vars[ 'Style' ][ '33:Hits_Per_Page' ]                                   = array ( "Number of listings to show per page" , "TEXT" , "25" , "design" );
				$vars[ 'Style' ][ '40' ]                                                 = array ( "Content Match Display Settings" , "HEADER" , "" , "design" );
				$vars[ 'Style' ][ '41:content_match_header_bg_color' ]                   = array ( "Header Background Color" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '42:content_match_header_text_color' ]                 = array ( "Header Text Color" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '43:content_match_border_color' ]                      = array ( "Border Color" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '44:content_match_cell_color' ]                        = array ( "Color of Result Cells" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '45:content_match_width' ]                             = array ( "Width the output" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '46:content_match_height' ]                            = array ( "Height of the output" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '47:content_match_text_color' ]                        = array ( "Color of Result Text" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '48:content_match_link_color' ]                        = array ( "Color of Result Links" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '49:content_match_num_results' ]                       = array ( "# Results to return" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '50:content_match_num_columns' ]                       = array ( "# of Columns of results <br />Note: The number of results returned will be limited to the number of columns if you choose to display more than one column)" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '51:content_match_powered_by' ]                        = array ( "Powered By Text" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '52:content_match_powered_by_location' ]               = array ( "Powered By Text Location" , "SELECT" , "top|bottom|suppress" , "design" );
				$vars[ 'Style' ][ '53:content_match_font' ]                              = array ( "Font Name/Family for CM Output" , "TEXT" , "" , "design" );
				$vars[ 'Style' ][ '54:content_match_font_size' ]                         = array ( "Font Size (css compat)" , "TEXT" , "" , "design" );
				$vars[ 'hyperseek_settings' ][ '07:yh_default_title' ]                   = array ( "Default META Title Value" , "TEXT" , "My Search Engine" , "design" );
				$vars[ 'hyperseek_settings' ][ '08:yh_default_desc' ]                    = array ( "Default META Description Value" , "TEXT" , "Powered by the Hyperseek Search Engine: http://www.hyperseek.com" , "design" );
				$vars[ 'hyperseek_settings' ][ '09:yh_default_keys' ]                    = array ( "Default META Keywords Value" , "TEXT" , "hyperseeek,search engine,search engine software,iweb" , "design" );
				$vars[ 'hyperseek_settings' ][ '03:yh_cool' ]                            = array ( "At what point is a listing <B>cool</B>" , "TEXT" , "5" );
				$vars[ 'hyperseek_settings' ][ '05:yh_new_days' ]                        = array ( "How many days old are links still considered new" , "TEXT" , "7" );
				$vars[ 'hyperseek_settings' ][ '06:yh_new_display' ]                     = array ( "Show whats new listings on a single screen or breadk down by date" , "SELECT" , "SHOW ALL|BY DATE" , "design" );
				$vars[ 'hyperseek_settings' ][ '07:yh_new_return' ]                      = array ( "If showing all new listings whats the max # to return" , "TEXT" , "20" );
				$vars[ 'hyperseek_settings' ][ '11:yh_target' ]                          = array ( "Target for all links" , "SELECT" , "_top|_blank|new|Frameset|CURRENT" , "design" );
				$vars[ 'hyperseek_settings' ][ '13:yh_show_hits' ]                       = array ( "Track hits for each site" , "SELECT" , "YES|NO" );
				$vars[ 'hyperseek_settings' ][ '14:yh_expired' ]                         = array ( "Will you be using expiring links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_settings' ][ '20:rate_min_votes' ]                     = array ( "Minimum number of votes before ratings are reported" , "TEXT" , "3" );
				$vars[ 'hyperseek_settings' ][ '21:yh_top' ]                             = array ( "For the TOP 10 Plugin, how many listings to report" , "TEXT" , "10" , "design" );
				$vars[ 'hyperseek_settings' ][ '22:top_cookie_days' ]                    = array ( "For Top10 and Ratings, how many days to allow between votes" , "TEXT" , "7" );
				$vars[ 'hyperseek_settings' ][ '25:yh_javascript' ]                      = array ( "Use JavaScript windows for Comments/Rate-It" , "SELECT" , "YES|NO" , "design" );
				$vars[ 'hypersek_category_settings' ][ '00:emulate_static_pages' ]       = array ( "Emulate Static HTML URLs?<br /><i>Requires 'PATH INFO' support ... might cause broken URLs for categories with non-alphanumeric characters ...</i>" , "SELECT" , "NO|YES" , "design" );
				$vars[ 'hypersek_category_settings' ][ '01:jh_categories_as_terms' ]     = array ( "Treat category lookups as search terms?<br /><i>You will need to create each category with a value (single search term or phrase) in it's <b>META KEYWORDS</b> field. </i>" , "SELECT" , "NO|YES" , "content" );
				$vars[ 'hypersek_category_settings' ][ '02:use_combined' ]               = array ( "For smaller categories use combined results" , "SELECT" , "YES|NO" , "design" );
				$vars[ 'hypersek_category_settings' ][ '11:yh_cat_templ' ]               = array ( "Would you like subcategory templates to match parent category templates" , "SELECT" , "YES|NO" , "design" );
				$vars[ 'hypersek_category_settings' ][ '12:yh_sort_cats' ]               = array ( "How should the categories be sorted?" , "SELECT" , "Manual Sort Order|Alphabetically" , "design" );
				$vars[ 'hypersek_category_settings' ][ '33:yh_sort' ]                    = array ( "How would you like the CATEGORY listings sorted" , "SELECT" , "RANK|TITLE|BOTH|SCORE|SCORE then RANK|SCORE then RANDOM|RANDOM" );
				$vars[ 'hyperseek_add_url_settings' ][ '01:add_url_spam_time' ]          = array ( "How long should the spam timer give visitors to complete the add url process(in minutes)" , "TEXT" , "20" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '02:yh_accounts' ]                = array ( "Require users to have accounts before adding/modifying links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '03:yh_add_link_balance' ]        = array ( "Require users to have made a deposit (have a balance) before adding links?" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '08:yh_add_direct' ]              = array ( "Allow users with accounts to directly add links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '09:yh_change_direct' ]           = array ( "Allow users with accounts to directly change links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '10:yh_add_url_spider' ]          = array ( "Force the use of the spider to add links" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '10:yh_spider_timeout' ]          = array ( "When using the spider, how long should the timeout period be (in seconds)?" , "TEXT" , "10" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '12:Default_Rank' ]               = array ( "Default ranking for new listings" , "TEXT" , "3" );
				$vars[ 'hyperseek_add_url_settings' ][ '13:yh_max_keywords' ]            = array ( "Maximum number of keywords allowed" , "TEXT" , "100" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '14:yh_max_desc' ]                = array ( "Maximum number of characters allowed in the description" , "TEXT" , "500" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '16:yh_verify_links' ]            = array ( "Do you want all new submissions verified" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '17:yh_parse_remote' ]            = array ( "Do you want to verify that all submitted links contain certain text" , "SELECT" , "YES|NO" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '18:yh_verify_text' ]             = array ( "If so, the text to search for" , "TEXT" , "" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '19:yh_domains' ]                 = array ( "Adding a listing can only be done from the following domains" , "TEXT" , "" );
				$vars[ 'hyperseek_add_url_settings' ][ '20:yh_add_html' ]                = array ( "How should we handle HTML CODE in new submissions" , "SELECT" , "ALLOW|DISALLOW|IGNORE|SHOWCODE" );
				$vars[ 'hyperseek_add_url_settings' ][ '21:yh_max_new' ]                 = array ( "Number of submissions to show per page in the admin" , "TEXT" , "10" , "content" );
				$vars[ 'hyperseek_add_url_settings' ][ '22:yh_add_dups' ]                = array ( "Check for duplicates when reviewing new submissions" , "SELECT" , "STRICT|DOMAIN|NO" , "content" );
				$vars[ 'hypersek_searching_options' ][ '05:yh_adult_filter' ]            = array ( "Turn 'Adult Filter' on?" , "SELECT" , "YES|NO" );
				$vars[ 'hypersek_searching_options' ][ '01:fti_fields' ]                 = array ( "List of fields from hyperseek_links (comma separated) indicating which fields to search for relevancy matching<br /><i>Note: this must match <b>exactly</b> the fulltext index on that table</i>" , "TEXT" , "title,description,keywords" );
				$vars[ 'hypersek_searching_options' ][ '02:sql_use_boolean' ]            = array ( "Use Boolean Mode for raw searching (mySQL 4 users only)" , "SELECT" , "NO|YES" );
				$vars[ 'hypersek_searching_options' ][ '03:sql_use_expansion' ]          = array ( "Use Query Expansion for raw searching single word queries (mySQL 4 users only)" , "SELECT" , "NO|YES" );
				$vars[ 'hypersek_searching_options' ][ '04:yh_search_sort' ]             = array ( "How would you like the SEARCH RESULTS sorted" , "SELECT" , "SCORE|TITLE|RANK|ALPHA-RANK|RANK then SCORE|SCORE then RANK|SCORE then RANDOM|CATEGORY|RANDOM" );
				$vars[ 'hypersek_searching_options' ][ '10:sorting_ppc_base' ]           = array ( "For <b>SCORE</b> Sorting, enter the base score to assign to <b>your Paid Listings</b>" , "TEXT" , "300" );
				$vars[ 'hypersek_searching_options' ][ '11:sorting_backfill_base' ]      = array ( "For <b>SCORE</b> Sorting, enter the base score to assign to <b>Backfill Paid Listings</b>" , "TEXT" , "150" );
				$vars[ 'hypersek_searching_options' ][ '11:sorting_backfill_free_base' ] = array ( "For <b>SCORE</b> Sorting, enter the base score to assign to <b>Backfill FREE Listings</b>" , "TEXT" , "5" );
				$vars[ 'hypersek_searching_options' ][ '12:sorting_free_base' ]          = array ( "For <b>SCORE</b> Sorting, enter the base score to assign to <b>your FREE Listings</b>" , "TEXT" , "50" );
				$vars[ 'hypersek_searching_options' ][ '21:yh_ignore' ]                  = array ( "Ignore the following words in searches and indexing" , "TEXT" , "of in on for to at by and the why" );
				$vars[ 'hypersek_searching_options' ][ '25:yh_qualify' ]                 = array ( "Would you like to offer a qualifying (Refined) search option" , "SELECT" , "Yes|No" , "content" );
				$vars[ 'hypersek_searching_options' ][ '50' ]                            = array ( "Supplemental / Default Results Options" , "HEADER" , "" , "content" );
				$vars[ 'hypersek_searching_options' ][ '51:default_search_term' ]        = array ( "Default Search Term (rerun search on this when no results found)" , "TEXT" , "" , "content" );
				$vars[ 'hyperseek_search_logging' ][ '61:yh_logging' ]                   = array ( "Turn 'Search Logs' on?" , "SELECT" , "YES|NO" );
				$vars[ 'hyperseek_search_logging' ][ '62:jackhammer_log_debug' ]         = array ( "Log click debug info (can become a HUGE Database table)" , "SELECT" , "NO|YES" );
				$vars[ 'hyperseek_search_logging' ][ '63:jackhammer_merge_tables' ]      = array ( "Use Monly 'Merge' tables for PPC click logging. (mySQL4 Required)" , "SELECT" , "NO|YES" );
				$vars[ 'templates' ][ 'LAYOUT' ]                                         = "SEARCHMENU|HOMEPAGE|HOMEPAGE SUBS|INSIDECATEGORIES|MENU|MENU SUBS";
				$vars[ 'templates' ][ 'LISTING' ]                                        = "link";
				$vars[ 'templates' ][ 'EMAIL' ]                                          = "Approved|Rejected";
				$page_templates[ 'pages' ]                                               = "default|directory|add-url|cool|newmenu|top10|rate|rate-report|comments|search|hits-report|recent-hits|recent-searches|top-searches|top-cats|top-visitors|top-referers|review|detail|moderators|inline|xml";
				## Generate the dynamic admin DHTML Menu
				if ( $ADMIN_TYPE == "designer" || $ADMIN_TYPE == "affiliate" ) {
						return;
				}
				list ( $NEW , $CHG , $SPD , $DEP , $BIDS , $AFF , $WITH , $TTL ) = queue_totals ();
				// Admin and Content Manager Links
				if ( preg_match ( "/content|administrator/" , $ADMIN_TYPE ) ) {
						$menuitems[ 'hyperseek' ][ 'title' ]                                                              = $language[ 'hs_admin' ];
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'tab_main' ] ]                              = 1;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_main' ] ][ $language[ 'directory_link' ] ]  = "$SCRIPT_NAME?admin=hyperseek&action=directory";
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'tab_queue' ] ]                             = 2;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ $language[ 'queue_add_link' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=edit_link";
						if ( $NEW >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_new_link] ($NEW)" ] = "$SCRIPT_NAME?admin=hyperseek&action=submissions&type=NEW";
						}
						if ( $CHG >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_changed_link] ($CHG)" ] = "$SCRIPT_NAME?admin=hyperseek&action=submissions&type=CHANGE";
						}
						if ( $SPD >= 1 ) {
								$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[queue_spidered_link] ($SPD)" ] = "$SCRIPT_NAME?admin=hyperseek&action=submissions&type=SPIDERED";
						}
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_queue' ] ][ "$language[spider_more_link]" ]                = "$SCRIPT_NAME?admin=hyperseek&action=spider_form";
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'tab_categories' ] ]                                       = 3;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_categories' ] ][ $language[ 'category_qualifiers_link' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=category_qualifiers";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_categories' ] ][ $language[ 'category_count_link' ] ]      = "$SCRIPT_NAME?admin=hyperseek&action=category_count";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_categories' ] ][ $language[ 'category_validate_link' ] ]   = "$SCRIPT_NAME?admin=hyperseek&action=category_validate";
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'tab_database' ] ]                                         = 4;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_database' ] ][ $language[ 'db_import_link' ] ]             = "$SCRIPT_NAME?admin=hyperseek&action=import_listings";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_database' ] ][ $language[ 'util_init_top10_link' ] ]       = "$SCRIPT_NAME?admin=hyperseek&action=init_top10";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_database' ] ][ $language[ 'util_init_ratings_link' ] ]     = "$SCRIPT_NAME?admin=hyperseek&action=init_ratings";
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'util_reports_header' ] ]                                  = 5;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'util_dead_link' ] ]      = "$SCRIPT_NAME?admin=hyperseek&action=view_verify_report";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_reports_header' ] ][ $language[ 'util_duplicate_link' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=duplicate_listings";
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'util_link_header' ] ]                                     = 7;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ $language[ 'util_reviews_link' ] ]      = "$SCRIPT_NAME?admin=hyperseek&action=review_list";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ $language[ 'util_featured_link' ] ]     = "$SCRIPT_NAME?admin=hyperseek&action=list_featured";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ $language[ 'util_expired_link' ] ]      = "$SCRIPT_NAME?admin=hyperseek&action=expired_links";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ $language[ 'new_link' ] ]               = "$SCRIPT_NAME?admin=hyperseek&action=search&search=NEW";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ $language[ 'cool_link' ] ]              = "$SCRIPT_NAME?admin=hyperseek&action=search&search=COOL";
						/*Menu itmes added by Akhilesh on 1/4/2011*/
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ 'Add Global Listing(RON)' ] = "$SCRIPT_NAME?admin=hyperseek&action=add_link";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_link_header' ] ][ 'Global Listings' ]         = "$SCRIPT_NAME?admin=hyperseek&action=list_globaled";
				}
				// Admin Only Links
				if ( preg_match ( "/administrator/" , $ADMIN_TYPE ) ) {
						$menuitems[ 'hyperseek' ][ 'categories' ][ $language[ 'util_email_header' ] ]                                        = 6;
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_email_header' ] ][ $language[ 'util_webmaster_email_link' ] ] = "$SCRIPT_NAME?admin=hyperseek&action=webmaster_mailing";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'util_email_header' ] ][ $language[ 'util_accounts_link' ] ]        = "$SCRIPT_NAME?admin=hyperseek&action=user_accounts&list=1";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_database' ] ][ $language[ 'db_export_link' ] ]                 = "$SCRIPT_NAME?admin=hyperseek&action=export_listings";
						$menuitems[ 'hyperseek' ][ 'items' ][ $language[ 'tab_database' ] ][ $language[ 'db_mass_delete_link' ] ]            = "$SCRIPT_NAME?admin=hyperseek&action=mass_delete";
				}
		}

		## Helper Function ##
		function queue_totals ()
		{
				global $oSQL;
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'NEW'";
				$submissions = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'CHANGE'";
				$changes     = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT count(*) FROM " . TEMP_TABLE . " WHERE submission_type = 'SPIDERED'";
				$spidered    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT count(*) FROM " . LINKS_TABLE;
				$total       = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$deposits    = 0;
				$pendingbids = 0;
				$affiliates  = 0;
				$affiliates  = 0;
				$SQL         = "SELECT COUNT(*) FROM " . WITHDRAWLS_TABLE;
				$withdrawls  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . DEPOSITS_TABLE;
				$deposits    = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . PENDING_BIDS_TABLE;
				$pendingbids = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$SQL         = "SELECT COUNT(*) FROM " . GROUP_ASSIGNMENTS_TABLE . " WHERE group_name LIKE 'Pending%'";
				$affiliates  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				if ( !$submissions ) {
						$submissions = 0;
				}
				if ( !$changes ) {
						$changes = 0;
				}
				if ( !$spidered ) {
						$spidered = 0;
				}
				if ( !$total ) {
						$total = 0;
				}
				if ( !$deposits ) {
						$deposits = 0;
				}
				if ( !$pendingbids ) {
						$pendingbids = 0;
				}
				if ( !$affiliates ) {
						$affiliates = 0;
				}
				if ( !$withdrawls ) {
						$withdrawls = 0;
				}

				return array ( $submissions , $changes , $spidered , $deposits , $pendingbids , $affiliates , $withdrawls , $total );
		}

?>