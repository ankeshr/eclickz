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


		#--------------------------------------------------------------------------#
		#  Category Browser frameset                                               #
		#--------------------------------------------------------------------------#
		function directory ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$JS        = '';
				$SQL       = "SELECT count(*) FROM " . CATEGORY_TABLE;
				$numcats   = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				$full_tree = 0;
				# if ( $numcats <= 2000 ) { $full_tree = 1; }
				if ( $full_tree ) {
						$SQL  = "SELECT category_id, parent_id, category_name FROM " . CATEGORY_TABLE . " ORDER BY parent_id, category_id";
						$cats = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $cats as $cat ) {
								## Moderator Check
								if ( $moderator && !$moderator_categories[ $cat[ category_id ] ] ) {
										continue;
								}
								## Invalid ID Check
								if ( $cat[ category_id ] < 0 || $cat[ parent_id ] < 0 ) {
										continue;
								}
								$cat[ category_name ] = preg_replace ( "/\'/" , "\\\'" , $cat[ category_name ] );
								if ( $cat[ parent_id ] == 0 ) {
										$JS .= "
                           nodes[$cat[category_id]] = new WebFXTreeItem('$cat[category_name]','javascript:switchCat($cat[category_id])');
                           tree.add(nodes[$cat[category_id]]);
                   ";
								} else {
										$JS .= "
                           nodes[$cat[category_id]] = new WebFXTreeItem('$cat[category_name]','javascript:switchCat($cat[category_id])');
                           nodes[$cat[parent_id]].add(nodes[$cat[category_id]]);
                   ";
								}
						}
				} else {
						$category = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						$name     = $category->data[ full_name ];
						$children = $category->children ( array ( "category_id" => 0 ) );
						## Create Each Child in the nodes array ...
						$carr = array ();
						foreach ( $children as $ccat ) {
								array_push ( $carr , $ccat->data );
						}
						aasort ( $carr , "+category_id" );
						foreach ( $carr as $ccat ) {
								$cat_id   = $ccat[ category_id ];
								$cat_name = $ccat[ category_name ];
								## Moderator Check
								if ( $moderator && !$moderator_categories[ $cat_id ] ) {
										continue;
								}
								$cat_name = preg_replace ( "/\'/" , "\\\'" , $cat_name );
								$JS .= "
                       nodes[$cat_id] = new WebFXTreeItem('$cat_name','javascript:switchCat($cat_id)');
                       tree.add(nodes[$cat_id]);
               ";
						}
				}
				$QS = '';
				if ( $CGI[ Terms ] || $CGI[ search ] ) {
						foreach ( $CGI as $var => $val ) {
								if ( $var == "action" ) {
										continue;
								}
								$evalue = urlencode ( $val );
								$QS .= $var . "=" . $evalue . "&";
						}
				}
				echo "
            <html>
              <head>

                <script language='javascript'>
                    var image_dir = '$config[button_dir]';
                </script>

                <script src='$config[script_dir]/xtree-hs.js'></script>

                <script language='javascript'>

                    var tree = new WebFXTree('Root');
                    tree.setBehavior('explorer');
                    var nodes = new Array;

                    $JS

                </script>

              </head>

              <iframe height='98%' width='100%' name='cats' src='$SCRIPT_NAME?admin=$CGI[admin]&action=directory_cats&full_tree=$full_tree&$QS' scrolling='no' frameborder='no'></iframe>

            </html>

        ";
				exit;
		}


		#--------------------------------------------------------------------------#
		#  Category Browser left side content  (javascript)                        #
		#--------------------------------------------------------------------------#
		function directory_cats ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				global $admin;
				$full_tree = $CGI[ full_tree ];
				$JS        = '';
				if ( $CGI[ category ] ) {
						$CGI[ category_id ] = $CGI[ category ];
				}
				if ( $CGI[ category_id ] ) {
						if ( $full_tree ) {
								$JS = '';
						} else {
								$category = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
								$name     = $category->data[ full_name ];
								$children = $category->children ( array ( "category_id" => $category->data[ category_id ] ) );
								if ( $children ) {
										## Create Each Child in the nodes array ...
										$carr = array ();
										foreach ( $children as $ccat ) {
												array_push ( $carr , $ccat->data );
										}
										aasort ( $carr , "+sort_order" );
										foreach ( $carr as $ccat ) {
												$cat_id   = $ccat[ category_id ];
												$cat_name = $ccat[ category_name ];
												## Moderator Check
												if ( $moderator && !$moderator_categories[ $cat_id ] ) {
														continue;
												}
												$cat_name = preg_replace ( "/\'/" , "\\\'" , $cat_name );
												$JS .= "
                         if ( ! parent.nodes[$cat_id] ) {
                           parent.nodes[$cat_id] = new parent.WebFXTreeItem('$cat_name','javascript:switchCat($cat_id)');
                           parent.nodes[$CGI[category_id]].add(parent.nodes[$cat_id]);
                         }
                      ";
										}
								}
						}
						$expand = " parent.nodes[" . $CGI[ category_id ] . "].expand(); ";
				}
				list ( $content , $menu ) = $admin->directory_content ();
				$OUTPUT = "

                <style>
                    body { background: white; color: black; }
                    input { width: 120px; }
                    .webfx-tree-container     { margin: 0px; padding: 0px; font: icon; }
                    .webfx-tree-item          { padding: 0px; margin: 0px; font: icon; color: black; }
                    .webfx-tree-item a        { color: black; text-decoration: none; margin-left: 5px; }
                    .webfx-tree-item a:hover  { color: blue; text-decoration: underline; margin-left: 5px; }
                    .webfx-tree-item a:active { background: highlight; color: white; text-decoration: none; margin-left: 5px; }
                    .webfx-tree-item img      { vertical-align: middle; }
                </style>

                <script language='javascript'>
                    if ( top.frames['blueframe'] ) {
                        var NS4 = (document.layers) ? 1 : 0;
                        var IE = (document.all) ? 1 : 0;
                        NS = (navigator.appName=='Netscape' && navigator.plugins['LiveAudio'])? 1:0;
                        var DOM = 0;
                        if (parseInt(navigator.appVersion) >=5) {DOM=1};
                        var ver4 = IE||NS? 1:0;
                        var content = '<body style=\"margin:0px; padding:0px; border:1px solid silver;background:silver\"> <table width=100% hspace=0 vspace=0><tr> <td valign=\"top\" align=\"left\" style=\"font-family:verdana,arial,helvetica; font-size:9pt; font-weight:bold;color:navy\">$language[search_results_header] <b>$name </b></td><td valign=\"top\" align=\"right\"><a href=\"javascript:openHelp();\"> <img src=\"$config[button_dir]/help-icon.gif\" border=0></a></td></tr></table></body>';

                        if (DOM) {
                           var cdiv = top.blueframe.document.getElementById('bluecontent');
                        }
                        else if (IE) {
                           var cdiv = top.blueframe.document.all['bluecontent'];
                        }
                        cdiv.innerHTML = content;
                   }
                </script>

                <script language='javascript'>

                   function switchCat(id) {
                      self.location.href='$SCRIPT_NAME?admin=$CGI[admin]&action=directory_cats&full_tree=$full_tree&category_id=' +  id;
                   }

                   $JS

                </script>


            </blockquote>

                <div style='position: absolute; top: 0px; left: 0px; width: 100%; padding: 2px; overflow: hidden; border:1px solid black'>
                $menu
                </div>

                <div style='position: absolute; width: 100%; top: 25px; left: 0px; height: 100%; padding: 5px; overflow: auto;'>
                    <script language='javascript'>
                        if (document.getElementById) {
                           document.write(parent.tree);
                           $expand
                        }
                    </script>

                    <br /><br />
                    <a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=edit_category&parent_id=0\")'><FONT FACE='arial,helvetica' SIZE=-1 COLOR='black'>$language[add_subcategory]</FONT></a>

                    <div style='position: absolute; left: 205px; top: 35px;'>
                         $content
                    </div>

                </div>

                <script language='javascript'>

                     var NS4 = (document.layers) ? 1 : 0;
                     var IE = (document.all) ? 1 : 0;
                     var DOM = 0;
                     if (parseInt(navigator.appVersion) >=5) {DOM=1};

                     var height = 640;

                     if (parseInt(navigator.appVersion)>3) {
                         height = window.innerHeight;
                         if ( document.all ) { height = parent.document.body.offsetHeight; }
                     }

                     height = height - 230;

                     document.write('<div id = \"workframe\" style=\"position:absolute; top:100px; left:50px;z-index:5;visibility:hidden; border:1px ridge silver;\"><table width=\"85%\" cellspacing=\"1\" cellpadding=\"2\" style=\"background:#596E88;color:white\"><tr><td><a style=\"color:white\" href=\"javascript:close_frame()\">Close</a></td></tr><tr><td><iframe style=\"z-index:5; overflow:auto;\" name=\"wrk\" width=\"100%\" height=\"' + height + '\" src=\"\"></iframe></td></tr></table></div>');

                     function load_url( url ) {
                         if ( DOM ) {
                             var cont = document.getElementById('workframe');
                             wrk.location.href=url;
                             cont.style.visibility = 'visible';
                         }
                         else if (IE) {
                             var frame = document.all['wrk'];
                             var cont = document.all['workframe'];
                             frame.src=url;
                             cont.style.visibility = 'visible';
                         }
                     }
                     function close_frame( url ) {
                         if ( DOM ) {
                             var cont = document.getElementById('workframe');
                             cont.style.visibility = 'hidden';
                         }
                         else if (IE) {
                             var cont = document.all['workframe'];
                             cont.style.visibility = 'hidden';
                         }
                     }
                     function open_frame() {
                         if ( DOM ) {
                             var cont = document.getElementById('workframe');
                             cont.style.visibility = 'visible';
                         }
                         else if (IE) {
                             var cont = document.all['workframe'];
                             cont.style.visibility = 'visible';
                         }
                     }

                 </script>

       ";
				admin_output ( "" , $OUTPUT );
		}


		#--------------------------------------------------------------------------#
		#  Category Management                                                     #
		#--------------------------------------------------------------------------#

		function category_home ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$category = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name     = $category->data[ full_name ];
				$tree     = category_navigator ( array ( "command" => 'category_home' , "category_id" => $CGI[ category_id ] ) );
				$menu     = category_menu ( $name , "list" );
				$OUTPUT   = "
          <table border=0 cellspacing=20>
           <tr bgcolor='#e0e0e0'>
             <td colspan=2>$language[category_home_current] <b>$name</b></td>
           </tr>
           <tr>
            <td valign='top'>
               $tree
            </td>
            <td valign='top'>
               $menu
            </td>
           </tr>
          </table>
       ";
				admin_output ( $language[ category_home_title ] , $OUTPUT );
		}

		#--------------------------------------------------------------------------#
		#  Creates a navigation menu for each category                             #
		#--------------------------------------------------------------------------#
		function category_menu ( $name = '' , $format = '' )
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				if ( $CGI[ admin ] == "navigator" ) {
						return;
				}
				$encname        = cleanup_string ( $name , "-" , 1 );
				$hyperseek      = '';
				$hyperseek_list = '';
				$listings       = '';
				if ( $CGI[ admin ] == "hyperseek" ) {
						$hyperseek = "
                   <td>
                     <a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=edit_link&category_id=$CGI[category_id]\")'>$language[category_menu_add_link]</a>
                   </td>
                   <td>
                     <a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=list_moderators&category_id=$CGI[category_id]\")'>$language[category_menu_moderators]</a>
                   </td>
            ";
						$listings  = $language[ category_menu_listings_links ];
				} else {
						$listings = $language[ category_menu_listings_nav ];
				}

				return "
           <table width=100% cellpadding=0 cellspacing-0 bgcolor='white' border=0>
             <tr bgcolor='white'>
               <td><b>$language[catmenu_header]</b></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=edit_category&category_id=$CGI[category_id]\")'>$language[catmenu_settings]</a></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=category_subs&category_id=$CGI[category_id]\")'>SUBS</a></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=move_category&move_id=$CGI[category_id]\")'>$language[catmenu_move]</a></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=delete_category&category_id=$CGI[category_id]\")'>$language[catmenu_delete]</a></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=edit_category&parent_id=$CGI[category_id]\")'>$language[catmenu_add_subcat]</a></td>
               <td align='center'><a href='javascript:load_url(\"$SCRIPT_NAME?action=edit_file&filename=$encname\")'>$language[category_menu_template]</a></td>
               $hyperseek
             </tr>
           </table>
        ";
		}

		#--------------------------------------------------------------------------#
		#  Creates a navigation menu for each category                             #
		#--------------------------------------------------------------------------#
		function category_navigator ( $args = array () )
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$command     = $args[ command ];
				$category_id = $args[ category_id ];
				$parent_id   = $args[ category_id ];
				$move_id     = $args[ move_id ];
				$acct_id     = $args[ acct_id ];
				$admin       = $CGI[ admin ];
				if ( $acct_id ) {
						$action .= "&acct_id=$acct_id";
				}
				$category = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $category_id ) );
				## If we're browsing, create a category object for later use.
				if ( $category_id > 0 ) {
						$children = $category->children ( array ( "category_id" => $category->data[ category_id ] ) );
						$parents  = $category->parents ( array ( "category_id" => $category->data[ category_id ] ) );
				} else {
						$children = $category->children ( array ( "category_id" => 0 ) );
						$parents  = $category->parents ( array ( "category_id" => 0 ) );
				}
				## Draw each parent ...
				$tree    = " <img src='$config[button_dir]/folder.gif' />&nbsp;<a href='$SCRIPT_NAME?admin=$admin&action=$command'>Top</a><br /> ";
				$spaces  = "&nbsp;&nbsp;";
				$parents = array_reverse ( $parents );
				if ( $category_id > 0 ) {
						foreach ( $parents as $cat ) {
								## Moderator Check
								if ( $moderator && !$moderator_categories[ $cat->data[ category_id ] ] ) {
										continue;
								}
								$catname = preg_replace ( "/ /" , "&nbsp;" , $cat->data[ category_name ] );
								$cid     = $cat->data[ category_id ];
								$pid     = $cat->data[ parent_id ];
								$action  = "$SCRIPT_NAME?admin=$admin&action=$command&move_id=$move_id&parent_id=$pid";
								$tree .= "$spaces <img src='$config[button_dir]/folder.gif' />&nbsp;<a href='$action&category_id=$cid'>$catname</a><br /> ";
								if ( $category_id ) {
										$spaces .= "&nbsp;&nbsp;";
								}
						}
				} else {
						foreach ( $parents as $cat ) {
								## Moderator Check
								if ( $moderator && !$moderator_categories[ $cat->data[ category_id ] ] ) {
										continue;
								}
								$catname = preg_replace ( "/ /" , "&nbsp;" , $cat->data[ category_name ] );
								$cid     = $cat->data[ category_id ];
								$pid     = $cat->data[ parent_id ];
								$action  = "$SCRIPT_NAME?admin=$admin&action=$command&move_id=$move_id&parent_id=$pid";
								$tree .= "$spaces <img src='$config[button_dir]/folder.gif' />&nbsp;<a href='$action&category_id=$cid'>$catname</a><br /> ";
								if ( $category_id ) {
										$spaces .= "&nbsp;&nbsp;";
								}
						}
				}
				## Draw Self
				$myparent = $category->data[ parent_id ];
				$myname   = preg_replace ( "/ /" , "&nbsp;" , $category->data[ category_name ] );
				$action   = "$SCRIPT_NAME?admin=$admin&action=$command&move_id=$move_id&parent_id=$myparent";
				$spaces .= "&nbsp;&nbsp;";
				$tree .= "$spaces <img src='$config[button_dir]/folder.gif' />&nbsp;<a href='$action&category_id=$category_id'>$myname</a><br /> ";
				## Draw each child ...
				foreach ( $children as $cat ) {
						## Moderator Check
						if ( $moderator && !$moderator_categories[ $ccat[ category_id ] ] ) {
								continue;
						}
						$cat->data[ category_name ] = preg_replace ( "/ /" , "&nbsp;" , $cat->data[ category_name ] );
						$catname                    = preg_replace ( "/ /" , "&nbsp;" , $cat->data[ category_name ] );
						$cid                        = $cat->data[ category_id ];
						$pid                        = $cat->data[ parent_id ];
						$action                     = "$SCRIPT_NAME?admin=$admin&action=$command&move_id=$move_id&parent_id=$pid";
						$tree .= "$spaces &#187;&nbsp;<a href='$action&category_id=$cid'>$catname</a><br /> ";
				}

				return ( $tree );
		}

		#--------------------------------------------------------------------------#
		#  Edit Teasers and sort order for subcategories                           #
		#--------------------------------------------------------------------------#
		function category_subs ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$category     = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name         = $category->data[ full_name ];
				$children     = $category->children ( array ( "category_id" => $category->data[ category_id ] ) );
				$menu         = category_menu ( $name );
				$OUT          = '';
				$numkids      = count ( $children ) + 1;
				$sort_options = '';
				for ( $x = 1; $x <= $numkids; $x++ ) {
						$sort_options .= " <option value='$x'>$x</option> ";
				}
				foreach ( $children as $child ) {
						$id          = $child->data[ category_id ];
						$teaser      = $child->data[ teaser ];
						$sortorder   = $child->data[ sort_order ];
						$catname     = $child->data[ category_name ];
						$teaser_form = "
                <select name='teaser:$id'>
                    <option value='$teaser'>$teaser</option>
                    <option value='1'>1</option>
                    <option value='0'>0</option>
                </select>
           ";
						$OUT .= "
             <tr>
               <td>$catname</td>
               <td>
                   <select name='sort_order:$id'>
                    <option value='$sortorder'>$sortorder</option>
                    $sort_options
                   </select>
               </td>
               <td>
                  $teaser_form
               </td>
             </tr>
           ";
				}
				$import_url = " <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=import_category&category_id=$CGI[category_id]'>$language[category_menu_import]</a> ";
				$formaction = '';
				if ( $nav ) {
						$import_url = " <a href='javascript:load_url(\"$SCRIPT_NAME?admin=$CGI[admin]&action=import_category&category_id=$CGI[category_id]\")'>$language[category_menu_import]</a> ";
						$formaction = 'target="wrk" onsubmit="open_frame();"';
				}
				$editor = "

          <br />

          <table border=0 cellspacing=0 cellpadding=3>
             <form action='$SCRIPT_NAME' method='post' $formaction>
             <tr>
                <th>Subcategory</th>
                <th>Sort Order</th>
                <th>Teaser?</th>
             </tr>
             $OUT
             <tr>
               <td colspan=2 align='center'>
                  <input type='hidden' name='action' value='save_category_subs'>
                  <input type='hidden' name='admin' value='$CGI[admin]'>
                  <input type='hidden' name='category_id' value='$CGI[category_id]'>
                  <input type='submit' value='$language[category_edit_button]'>
               </td>
             </tr>

             <tr>
                <td colspan=2 align='center'>
                  $import_url
                </td>
             </tr>
             </form>
          </table>

       ";
				if ( $nav ) {
						return $editor;
				}
				admin_output ( "$language[category_edit_title] <i>$name</i>" , $editor );
		}

		#--------------------------------------------------------------------------#
		#  Save Teasers and sort order for subcategories                           #
		#--------------------------------------------------------------------------#
		function save_category_subs ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$category = new CategoryNavigator ( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name     = $category->data[ full_name ];
				$menu     = category_menu ( $name );
				$message  = '';
				foreach ( $CGI as $key => $value ) {
						if ( preg_match ( "/sort_order/i" , $key ) ) {
								list ( $z , $id ) = split ( ":" , $key );
								$sort_order[ $id ] = $value;
						}
						if ( preg_match ( "/teaser/i" , $key ) ) {
								list ( $z , $id ) = split ( ":" , $key );
								$teasers[ $id ] = $value;
						}
				}
				ksort ( $sort_order );
				foreach ( $sort_order as $id => $value ) {
						$thiscat                     = new Record( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $id ) );
						$thisname                    = $thiscat->data[ category_name ];
						$thiscat->data[ sort_order ] = $sort_order[ $id ];
						$thiscat->data[ teaser ]     = $teasers[ $id ];
						$thiscat->update_values ( $thiscat->data );
						$thiscat->save ();
						$message .= "<tr bgcolor='#e0e0e0'><td>$name</td><td>$sort_order[$id]</td><td>$teasers[$id]</td>";
				}
				$OUTPUT = "
          <table border='0' cellspacing='0' cellpadding='3'>
             $menu
          </table>
          <table border='0' cellspacing='1' cellpadding='3'>
             <tr>
                <th>subcategory</th>
                <th>sort order</th>
                <th>teaser?</th>
             </tr>
             $message
          </table>
       ";
				admin_output ( "$language[category_save_title] <i>$name</i>" , $OUTPUT );
		}


		#--------------------------------------------------------------------------#
		#  Edit a category's settings                                              #
		#--------------------------------------------------------------------------#
		function edit_category ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$id = $CGI[ category_id ];
				## Set a new category_id, if this is an addition ...
				if ( array_key_exists ( "parent_id" , $CGI ) ) {
						$SQL = "SELECT max(category_id) FROM " . CATEGORY_TABLE;
						$max = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$max++;
						$CGI[ category_id ] = $max;
						$id                 = $CGI[ parent_id ];
				}
				if ( $moderator && !$moderator_categories[ $id ][ access_level ] != "Admin" ) {
						die( "You do not have permission to edit this category<br />" );
				}
				## Grab the record
				$category = new Record( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name     = $category->data[ full_name ];
				## Set a new parent_id, if this is an addition ...
				if ( array_key_exists ( "parent_id" , $CGI ) ) {
						$category->data[ parent_id ]   = $CGI[ parent_id ];
						$category->data[ category_id ] = $CGI[ category_id ];
				} else {
						$CGI[ parent_id ] = $category->data[ parent_id ];
						if ( !$CGI[ parent_id ] ) {
								$CGI[ parent_id ] = 0;
						}
				}
				$formarray  = $category->generate_form ();
				$formfields = '';
				while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
						$formfields .= "<tr><td bgcolor='#e0e0e0'>$prompt</td><td bgcolor='#c0c0c0'>$fieldinput</td></tr>\n";
				}
				## No menu if we're using Navigator
				$menu       = category_menu ( $name );
				$formaction = '';
				if ( $nav ) {
						$formaction = 'target="wrk" onsubmit="open_frame();"';
				}
				## Menu went in the output before the iframe change.
				$OUTPUT = "
          <br />
          <table border=0 cellspacing=0 cellpadding=3>
             <form action='$SCRIPT_NAME' method='post' $formaction>
             $formfields
             <tr>
               <td colspan=2 align='center'>
                  <input type='hidden' name='action' value='save_category'>
                  <input type='hidden' name='admin' value='$CGI[admin]'>
                  <input type='submit' value='$language[category_edit_button]'>
               </td>
             </tr>
             </form>
          </table>
       ";
				if ( $nav && !preg_match ( "/directory_/i" , $CGI[ action ] ) ) {
						return $output;
				}
				admin_output ( "$language[category_edit_title] <i>$name</i>" , $OUTPUT );
		}

		#--------------------------------------------------------------------------#
		#  Save category settings (Prints confirmation, or returns a value         #
		#--------------------------------------------------------------------------#
		function save_category ( $internal = '' )
		{
				global $iSQL;
				global $CGI;
				global $language;
				$category = new CategoryNavigator( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$message  = $category->save_cat ();
				$name     = $category->data[ full_name ];
				$menu     = category_menu ( $name );
				if ( $internal ) {
						return $message;
				}
				$OUTPUT = "
          <table>
            $menu
            <tr><td colspam=2>
               $message
            </td></tr>
          </table>
       ";
				admin_output ( "$language[category_save_title] <i>$name</i>" , $OUTPUT );
		}

		#--------------------------------------------------------------------------#
		#  Delete a category and delete/reassign its subs/links                    #
		#--------------------------------------------------------------------------#
		function delete_category ()
		{
				global $CGI;
				global $iSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$category = new CategoryNavigator( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name     = $category->data[ full_name ];
				## Do the delete ....
				if ( $CGI[ confirm ] ) {
						$OUTPUT = $category->delete_cat ();
				} ## .... Or give them a prompt.
				else {
						$OUTPUT = "
              $language[delete_category_description]
              <br /><br />
              &#187; <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=delete_category&category_id=$CGI[category_id]&confirm=1'>$language[delete_just_the_category]</a><P>
              &#187; <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=delete_category&category_id=$CGI[category_id]&confirm=1&deletesubs=1'>$language[delete_category_and_subs]</a><br /><br />
          ";
						if ( !$nav ) {
								$OUTPUT .= "
                 &#187; <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=delete_category&category_id=$CGI[category_id]&confirm=1&deletelinks=1'>$language[delete_category_and_links]</a><br /><br />
                 &#187; <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=delete_category&category_id=$CGI[category_id]&confirm=1&deletesubs=1&deletelinks=1'>$language[delete_all]</a><br /><br />
              ";
						}
				}
				admin_output ( "$language[category_delete_title] <i>$name</i>" , $OUTPUT );
		}


		#--------------------------------------------------------------------------#
		#  Change the parent category                                              #
		#--------------------------------------------------------------------------#
		function move_category ()
		{
				global $CGI;
				global $iSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				$category                              = new Record( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ move_id ] ) );
				$name                                  = $category->data[ category_name ];
				$cname                                 = urlencode ( $name );
				$target                                = new Record( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$tcat                                  = $target->data[ category_id ];
				$tname                                 = $target->data[ full_name ];
				$menu                                  = category_menu ( $name );
				$tree                                  = category_navigator ( array ( "command" => 'move_category' , "category_id" => $CGI[ category_id ] , "move_id" => $CGI[ move_id ] ) );
				$language[ move_category_description ] = preg_replace ( '/\$name/' , $name , $language[ move_category_description ] );
				$OUTPUT                                = "
         $language[move_category_description]
         <hr width='90%'>
         <table>
           <tr>
             <td>
                $tree
             </td>
           </tr>
           <tr bgcolor='#e0e0e0'>
             <td>
                $language[category_home_current]
                <b>$tname</b>
                <a href='$SCRIPT_NAME?admin=$CGI[admin]&action=save_category&category_id=$CGI[move_id]&parent_id=$tcat&category_name=$cname'>$language[move_to_this_level]</a>
             </td>
           </tr>
         </table>
       ";
				admin_output ( "$language[move_category_title] <i>$name</i>" , $OUTPUT );
		}


		#--------------------------------------------------------------------------#
		#  Import a predefined category tree                                       #
		#--------------------------------------------------------------------------#
		function import_category ()
		{
				global $CGI;
				global $oSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				global $data_dir;
				$category = new Record( array ( "Parent" => $oSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
				$name     = $category->data[ category_name ];
				$menu     = category_menu ( $name );
				$message  = '';
				if ( $CGI[ import_file ] || $CGI[ import_list ] ) {
						$message = do_category_import ();
				} else {
						$options = '';
						if ( $handle = opendir ( "$data_dir/hyperseek/global/" ) ) {
								while ( false !== ( $file = readdir ( $handle ) ) ) {
										if ( $file == "." || $file == ".." ) {
												continue;
										}
										$file = preg_replace ( "/\.global/" , "" , $file );
										$options .= " <option value='$file'>$file</option> ";
								}
								closedir ( $handle );
						}
						$message = "
             <form action='$SCRIPT_NAME' method='post'>
             $language[category_import_title] <i>$name</i><br /><br />
             <a href='$config[html_dir]/categories/importing.html'>$language[category_import_docs_link]</a><br /><br />
             <table width=400>
               <tr>
                 <td align='top'>
                    $language[category_import_file]
                 </td>
                 <td>
                    <select name='import_file'>
                      <option value=''>$language[category_import_select]</a>
                      $options
                    </select>
                 </td>
               </tr>
               <tr>
                 <td align='top'>
                    $language[category_import_manual]
                 </td>
                 <td>
                    <textarea name='import_list' cols=50 rows=8></textarea>
                 </td>
               </tr>
               <tr>
                 <td colspan=2 align='center'>
                    <input type='hidden' name='action' value='import_category'>
                    <input type='hidden' name='admin' value='$CGI[admin]'>
                    <input type='hidden' name='category_id' value='$CGI[category_id]'>
                    <input type='submit' value='$language[category_import_button]'>
                 </td>
               </tr>
             </table>
             </form>
          ";
				}
				$OUTPUT = "
          <table>
             $menu
             <tr>
               <td colspan='2'>
                 $message
               </td>
             </tr>
          </table>
       ";
				admin_output ( "$language[category_import_title] <i>$name</i>" , $OUTPUT );
		}

		#--------------------------------------------------------------------------#
		#  The actual Import Processing                                            #
		#--------------------------------------------------------------------------#
		function do_category_import ()
		{
				global $CGI;
				global $iSQL;
				global $SCRIPT_NAME;
				global $config;
				global $language;
				global $data_dir;
				$list = array ();
				if ( $CGI[ import_file ] ) {
						$contents = file ( "$data_dir/hyperseek/global/$CGI[import_file]" );
						foreach ( $contents as $line ) {
								$line = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $line );
								array_push ( $list , $line );
						}
				} else {
						$cats     = $CGI[ import_list ];
						$contents = preg_split ( "/\n|\r|\cM/" , $cats );
						foreach ( $contents as $line ) {
								$line = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $line );
								array_push ( $list , $line );
						}
				}
				## Note, that everything we do here, must use $CGI as the forcing issue, so that the save routine will work.
				## Get current category list
				$saved = array ();
				$SQL   = "SELECT full_name, category_id FROM " . CATEGORY_TABLE . " order by full_name";
				$saved = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "fullhash" ) );
				## Determine the starting ID
				$SQL = "SELECT max(category_id) FROM " . CATEGORY_TABLE;
				$max = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				## Default Parent ID
				$default_parent_id = $CGI[ category_id ];
				if ( !$default_parent_id ) {
						$default_parent_id = 0;
				}
				## Default "Full Name"
				$SQL               = "SELECT full_name FROM " . CATEGORY_TABLE . " WHERE category_id = $default_parent_id";
				$default_full_name = $iSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
				foreach ( $list as $category ) {
						$category = preg_replace ( "/^\s+|\s+$|\r|\n|\cM/" , "" , $category );
						if ( !$category ) {
								echo "Skipping (empty)<BR>\n";
								continue;
						}
						// My Category Name
						$contents      = split ( ":" , $category );
						$numc          = count ( $contents ) - 1;
						$category_name = $contents[ $numc ];
						// My Full Name
						if ( $default_full_name ) {
								$full_name = $default_full_name . ":" . $category;
						} else {
								$full_name = $category;
						}
						// My Parent's ID
						$parts       = preg_split ( "/\:/" , $full_name );
						$pparts      = array_pop ( $parts );
						$parent_name = join ( ':' , $parts );
						if ( $saved[ $parent_name ] ) {
								$parent_id = $saved[ $parent_name ];
						} else {
								$parent_id = $default_parent_id;
						}
						if ( $saved[ $full_name ] ) {
								echo "Skipping FN ... $full_name<BR>\n";
								continue;
						}
						$max++;
						$CGI                  = array ();
						$CGI[ parent_id ]     = "$parent_id";
						$CGI[ category_id ]   = $max;
						$CGI[ category_name ] = $category_name;
						$CGI[ full_name ]     = $full_name;
						$CGI[ teaser ]        = 0;
						$CGI[ cat_count ]     = 0;
						$CGI[ last_activity ] = time ();
						$CGI[ last_listing ]  = 1;
						$record               = new Record( array ( "Parent" => $iSQL , "Table" => CATEGORY_TABLE , "Schema" => CATEGORY_TABLE , "Lookup" => 1 , "category_id" => $CGI[ category_id ] ) );
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										echo "<b>Import OK</b> CID: $CGI[category_id] / PID: $CGI[parent_id] -> $category ($CGI[category_name])<br />\n";
								} else {
										echo "Error: $record->error_text<br />\n";
								}
						} else {
								echo "Error: $record->error_text<br />\n";
						}
						$saved[ $full_name ] = $max;
				}
				exit;
		}

?>
