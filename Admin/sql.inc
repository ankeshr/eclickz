<?php

		/*
				________________________________________________________________________________

					  	PROGRAM : ASP : SQL Schema Admin Center (PHP)
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


		class sqlAdmin
		{

				var $output;

				function sqlAdmin ()
				{
						// Includes & Setup
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


				// --------------------------------------------------------------------- //
				//  Drop list of all schemas an individual actions (main menu)
				// --------------------------------------------------------------------- //
				function list_schemas ( $header = '' )
				{
						global $oSQL;
						global $config;
						global $language;
						global $SCRIPT_NAME;
						$out    = "";
						$tables = "";
						$SQL    = "SELECT DISTINCT schema_name FROM " . SCHEMAS . " ORDER BY schema_name";
						$list   = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
						foreach ( $list as $schema_name ) {
								$tables .= " <option value='$schema_name'>$schema_name</option> ";
						}
						$OUT = "
             $header
             <br><br>
             <form action='$SCRIPT_NAME' method='post'>
             <table border=0 cellspacing=0 cellpadding=5 bgcolor='#e0e0e0'>
                 <tr bgcolor='silver'>
                   <th>$language[schema_table_prompt]</th>
                   <th colspan=4>$language[schema_actions_prompt]</th>
                 </tr>
                 <tr>
                    <td><select name='schema'>$tables</select></td>
                    <td>
                       <select name='action'>
                         <option value='quick_edit'>$language[schema_edit_prompt]</option>
                         <option value='regenerate'>$language[schema_regenerate_prompt]</option>
                         <option value='delete_schema'>$language[schema_delete_prompt]</option>
                         <option value='copy_schema'>$language[schema_copy_prompt]</option>
                       </select>
                    </td>
                 </tr>
                 <tr>
                   <td colspan=2 align='center'>
                     <input type='submit' value='$language[schema_go_button]'>
                     <input type='hidden' name='admin' value='sql'>
                   </td>
                 </tr>
                 <tr>
                   <td colspan=2>
                      <br><br>
                     <li><a href='$SCRIPT_NAME?admin=sql&action=generate_all' target='main'>$language[regenerate_link]</a>
                   </td>
                 </tr>

             </table>
             </form>

             <form action='$SCRIPT_NAME' method='post'>
                 $language[new_schema_prompt]  <input name='table'>
                 <input type='hidden' name='action' value='new_schema'>
                 <input type='hidden' name='admin' value='sql'>
                 <input type='submit' value='$language[schema_go_button]'>
             </form>

           ";
						admin_output ( $language[ schema_list_title ] , $OUT );
				}


				// --------------------------------------------------------------------- //
				//  Edit major schema settings (prompt, required, input type)
				// --------------------------------------------------------------------- //
				function quick_edit ( $header = '' )
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						global $config;
						$table   = $CGI[ schema ];
						$total   = 0;
						$output  = "";
						$options = "";
						$SQL     = "SELECT * FROM " . SCHEMAS . " WHERE schema_name = '$table' ORDER BY form_order";
						$fields  = $oSQL->search ( array ( "SQL" => $SQL ) );
						foreach ( $fields as $field ) {
								$total++;
								$order   = $field[ form_order ] + 1;
								$show    = preg_replace ( "/_/" , " " , $field[ field_name ] );
								$eschema = urlencode ( $table );
								$output .= "
                 <tr>
                   <td><a href='$SCRIPT_NAME?admin=sql&action=edit_field&field_name=$field[field_name]&schema=$eschema'>$show</td>

                   <td><textarea scrolling='no' name='form_prompt:$field[field_name]' cols='25' rows='1'>$field[form_prompt]</textarea></td>

                   <td align='center'>
                     <select name='form_order:$field[field_name]'>
                        <option value='$field[form_order]' selected>$order</option>
                        <<options>>
                     </select>
                   </td>

                   <td>
                       <select name='input_type:$field[field_name]'>
                          <option value='$field[input_type]' SELECTED>$field[input_type]</option>
                          <option value='TEXT BOX'>TEXT BOX</option>
                          <option value='LONG TEXT BOX'>LONG TEXT BOX</option>
                          <option value='TEXTAREA'>TEXTAREA</option>
                          <option value='BIG TEXTAREA'>BIG TEXTAREA</option>
                          <option value='RADIO'>RADIO</option>
                          <option value='CHECKBOX'>CHECKBOX</option>
                          <option value='SELECT SINGLE'>SELECT SINGLE</option>
                          <option value='SELECT MULTIPLE'>SELECT MULTIPLE</option>
                          <option value='UPLOAD'>UPLOAD</option>
                          <option value='HIDDEN'>HIDDEN</option>
                          <option value='HIDE AND SHOW'>HIDE AND SHOW</option>
                          <option value='DISPLAY ONLY'>DISPLAY ONLY</option>
                          <option value='IGNORE'>IGNORE</option>
                          <option value='CUSTOM'>CUSTOM</option>
                     </select>
                   </td>

                   <td>
                       <select name='is_required:$field[field_name]'>
                          <option value='$field[is_required]' selected>$field[is_required]</option>
                          <option value='YES'>YES</option>
                          <option value='NO'>NO</option>
                       </select>

                       <input type='hidden' name='fieldid:$field[field_name]' value='$field[field_name]'>
                   </td>

                 </tr>
              ";
						}
						for ( $x = 1; $x < $total; $x++ ) {
								$y = $x - 1;
								$options .= " <option value='$y'>$x</option> ";
						}
						$output = preg_replace ( "/(<<options>>)/" , $options , $output );
						$OUT    = "
             <h4>Current Table: $table</h4>
             $header
             <form action='$SCRIPT_NAME' method='post'>
               <table>
               <tr bgcolor='#e0e0e0'>
                 <th>$language[edit_schema_column]</th>
                 <th>$language[edit_schema_prompt]</th>
                 <th>$language[edit_schema_position]</th>
                 <th>input type</th>
                 <th>required?</th>
               </tr>
               $output
               <tr><td colspan=5>
                 <center>
                   <input type='hidden' name='action' value='quick_save'>
                   <input type='hidden' name='admin' value='sql'>
                   <input type='hidden' name='schema' value='$table'>
                   <input type='submit' value='$language[save_field_button]'>
                 </center>
               </td></tr>
              </table>
            </form>
           ";
						admin_output ( $language[ edit_schema_title ] , $OUT );
				}


				// --------------------------------------------------------------------- //
				//  Save schema settings (all in one shot)
				// --------------------------------------------------------------------- //
				function quick_save ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						$table       = $CGI[ schema ];
						$definitions = SCHEMAS;
						$SQL         = "
               UPDATE $definitions SET
                   form_order = ?,
                   form_prompt = ?,
                   is_required = ?,
                   input_type = ?
               WHERE schema_name = '$table'
               AND field_name = ?
           ";
						foreach ( $CGI as $field => $value ) {
								if ( !preg_match ( "/fieldid/i" , $field ) ) {
										continue;
								}
								$fieldname = $CGI[ $field ];
								$fo        = $CGI[ "form_order:$fieldname" ];
								$fp        = $CGI[ "form_prompt:$fieldname" ];
								$ir        = $CGI[ "is_required:$fieldname" ];
								$it        = $CGI[ "input_type:$fieldname" ];
								$fp        = preg_replace ( "/\'/" , "''" , $fp );
								$SQL       = "
                   UPDATE $definitions SET
                       form_order = '$fo',
                       form_prompt = '$fp',
                       is_required = '$ir',
                       input_type = '$it'
                   WHERE schema_name = '$table'
                   AND field_name = '$fieldname'
               ";
								$iSQL->run ( $SQL );
								$header .= " $fieldname $language[schema_moved_position] " . $CGI[ "form_order:$fieldname" ] . " (" . $CGI[ "form_prompt:$fieldname" ] . ")<br />";
						}
						$this->quick_edit ( $header );
				}


				// --------------------------------------------------------------------- //
				//  Edit a single field from a schema: more fine grained options
				// --------------------------------------------------------------------- //
				function edit_field ()
				{
						global $CGI;
						global $oSQL;
						global $SCRIPT_NAME;
						global $language;
						$table     = $CGI[ schema ];
						$record    = new Record( array ( 'Parent' => $oSQL , 'Table' => SCHEMAS , 'Schema' => SCHEMAS , 'Lookup' => 1 , 'schema_name' => $table , 'field_name' => $CGI[ field_name ] ) );
						$formarray = $record->generate_form ();
						$form      = '';
						while ( list ( $prompt , $fieldinput ) = each ( $formarray ) ) {
								$form .= "<tr><td>$prompt</td><td>$fieldinput</td></tr>\n";
						}
						$OUT = "
             <h4>$table</h4>
             <center>
             <form action='$SCRIPT_NAME' method='post'>
               <table>
                  $form
                  <tr>
                    <td colspan='2' align='center'>
                      <input type='hidden' name='action' value='save_field'>
                      <input type='hidden' name='admin' value='sql'>
                      <input type='hidden' name='table' value='$table'>
                      <input type='submit' value='$language[save_field_button]'>
                    </td>
                 </tr>
               </table>
             </form>
             </center>
           ";
						admin_output ( $language[ edit_field_title ] , $OUT );
				}


				// --------------------------------------------------------------------- //
				//  Save settings for a single field in a schema
				// --------------------------------------------------------------------- //
				function save_field ()
				{
						global $CGI;
						global $iSQL;
						global $SCRIPT_NAME;
						global $language;
						$record = new Record( array ( 'Parent' => $iSQL , 'Table' => SCHEMAS , 'Schema' => SCHEMAS , 'Lookup' => 1 , 'schema_name' => $CGI[ schema_name ] , 'field_name' => $CGI[ field_name ] ) );
						if ( $record->update_values ( $CGI ) ) {
								if ( $record->save () ) {
										$message = "<b><i>$CGI[schema_name]: $CGI[field_name]</i></b> ... Field Settings Saved";
								} else {
										$message = "<b><i>$CGI[schema_name]: $CGI[field_name]</i></b> ... <pre>$record->error_string</pre>";
								}
						} else {
								$message = "<b><i>$CGI[schema_name]: $CGI[field_name]</i></b> ... <pre>$record->error_string</pre>";
						}
						$CGI[ schema ] = $CGI[ schema_name ];
						$this->quick_edit ( $message );
				}


				// --------------------------------------------------------------------- //
				//  Re-Generate the core schema for every table in the database
				// --------------------------------------------------------------------- //
				function generate_all ()
				{
						global $CGI;
						global $language;
						global $SCRIPT_NAME;
						global $oSQL;
						ob_end_flush ();
						if ( $CGI[ confirm ] == 1 ) {
								echo "Generating Schemas ...<br><br>\n";
								flush ();
								$SQL    = "SHOW TABLES";
								$tables = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "array" ) );
								foreach ( $tables as $table ) {
										$status = $this->generate ( $table , $table );
										echo "Table: $table ($status)<br />\n";
										flush ();
								}
								echo "<br><br>Done\n";
								flush ();
								$this->list_schemas ( $report );
						} else {
								$OUT = "
                   <br><br>
                   $language[generate_all_warning]
                   <br><br>
                   <li><a href='$SCRIPT_NAME?admin=sql&action=generate_all&confirm=1' target='main'>$language[regenerate_link]</a>
               ";
								admin_output ( "Generate All" , $OUT );
						}
				}


				// --------------------------------------------------------------------- //
				//  Re-Generate a single schema
				// --------------------------------------------------------------------- //
				function regenerate ()
				{
						global $CGI;
						global $oSQL;
						$schema = $CGI[ schema ];
						$SQL    = "SELECT table_name FROM " . SCHEMAS . " WHERE schema_name = '$schema'";
						$table  = $oSQL->search ( array ( "SQL" => $SQL , "Return" => "single" ) );
						$result = $this->generate ( $table , $schema );
						if ( $result == 1 ) {
								$this->quick_edit ( "Schema for <b>$schema</b> Re-Generated" );
						}
				}


				// --------------------------------------------------------------------- //
				//  Create basic schema for a new table
				// --------------------------------------------------------------------- //
				function new_schema ()
				{
						global $CGI;
						$CGI[ schema ] = $CGI[ table ];
						$result        = $this->generate ( $CGI[ table ] , $CGI[ table ] );
						if ( $result == 1 ) {
								$this->quick_edit ( "Schema for <b>$CGI[table]</b> created" );
						}
				}


				// --------------------------------------------------------------------- //
				//  Copy an existing schema
				// --------------------------------------------------------------------- //
				function copy_schema ()
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $SCRIPT_NAME;
						$original = $CGI[ schema ];
						$new      = $CGI[ copy_to ];
						if ( $new ) {
								$status = $this->generate ( $original , $new , 1 );
								if ( $status == 1 ) {
										$header = "New Schema: <B>$new</B> Created, based on <I>$original</I>";
								} else {
										$header = "There was an error in creating this schema";
								}
								$this->quick_edit ( $header );
						} else {
								$OUT = "
                      $language[copy_schema_header] <b>$original</b><br><br>
                      <form action='$SCRIPT_NAME' method='post'>
                          $language[copy_schema_to] <input name='copy_to'>
                          <input type='hidden' name='schema' value='$original'>
                          <input type='hidden' name='action' value='copy_schema'>
                          <input type='hidden' name='admin' value='sql'>
                          <input type='submit' value='$language[copy_schema_button]'>
                      </form>
               ";
								admin_output ( $language[ schema_list_title ] , $OUT );
						}
				}


				// --------------------------------------------------------------------- //
				//  Delete a schema
				// --------------------------------------------------------------------- //
				function delete_schema ()
				{
						global $CGI;
						global $iSQL;
						$schema = $CGI[ schema ];
						$SQL    = "DELETE FROM " . SCHEMAS . " WHERE schema_name = '$schema'";
						$iSQL->run ( $SQL );
						$this->list_schemas ( "<b>$schema</b> Deleted" );
				}


				// --------------------------------------------------------------------- //
				//  Generate schema (used by copy, regenerate, generate_all)
				// --------------------------------------------------------------------- //
				function generate ( $table = '' , $schema_name = '' , $copy = 0 )
				{
						global $CGI;
						global $iSQL;
						global $language;
						global $config;
						$settings = array ();
						$number   = 1;
						if ( $table == SCHEMAS ) {
								return "Skipping";
						}
						if ( $copy ) {
								$SQL      = "SELECT * FROM " . SCHEMAS . " WHERE schema_name = '$table'";
								$settings = $iSQL->search ( array ( "SQL" => $SQL ) );
						} else {
								## First, get the existing schema, if any
								$SQL  = "SELECT * FROM " . SCHEMAS . " WHERE schema_name = '$schema_name'";
								$orig = $iSQL->search ( array ( "SQL" => $SQL ) );
								## Now, Read the table structure in, and append/update the original
								$SQL  = "DESCRIBE $table";
								$desc = $iSQL->search ( array ( "SQL" => $SQL ) );
								foreach ( $desc as $def ) {
										$fieldsettings                = array ();
										$fieldsettings[ table_order ] = $number;
										$fieldsettings[ form_order ]  = $number;
										$fieldsettings[ system ]      = 1;
										$fieldsettings[ field_name ]  = $def[ Field ];
										$fieldsettings[ form_prompt ] = strtoupper ( $def[ Field ] );
										if ( preg_match ( "/(.*)(\()(.*)(\))/" , $def[ Type ] , $m ) ) {
												$fieldsettings[ col_type ]   = $m[ 1 ];
												$fieldsettings[ col_length ] = $m[ 3 ];
												$fieldsettings[ col_type ]   = preg_match ( "/varchar/i" , "char" , $fieldsettings[ col_type ] );
												$fieldsettings[ col_type ]   = strtoupper ( $fieldsettings[ col_type ] );
										} else {
												$fieldsettings[ col_type ] = strtoupper ( $def[ Type ] );
										}
										if ( preg_match ( "/int/i" , $fieldsettings[ col_type ] ) ) {
												$fieldsettings[ default_value ] = "0";
										} elseif ( preg_match ( "/decimal/i" , $fieldsettings[ col_type ] ) ) {
												$fieldsettings[ default_value ] = "0.00";
										} else {
												$fieldsettings[ default_value ] = "";
										}
										$fieldsettings[ is_required ] = "N";
										foreach ( $orig as $original ) {
												if ( $original[ field_name ] != $fieldsettings[ field_name ] ) {
														continue;
												}
												foreach ( $original as $field => $value ) {
														if ( $field == "table_order" ) {
																continue;
														}
														if ( $field == "form_order" ) {
																continue;
														}
														if ( $field && $value ) {
																$fieldsettings[ $field ] = $value;
														}
												}
										}
										array_push ( $settings , $fieldsettings );
										$number++;
								}
						}
						$SQL = "DELETE FROM " . SCHEMAS . " WHERE schema_name = '$schema_name'";
						$iSQL->run ( $SQL );
						$errors = '';
						foreach ( $settings as $field ) {
								$field[ schema_name ] = $schema_name;
								$field[ table_name ]  = $table;
								$record               = new Record( array ( "Parent" => $iSQL , "Table" => SCHEMAS , "Schema" => SCHEMAS , "Lookup" => 1 , "table_name" => $table , "schema_name" => $schema_name , "field_name" => $field[ field_name ] ) );
								if ( $record->update_values ( $field ) ) {
										if ( $record->save () ) {
												$ok = 1;
										} else {
												$errors .= $record->error_string;
										}
								} else {
										$errors .= $record->error_string;
								}
						}
						if ( $errors ) {
								echo "ERR: <pre>$errors</pre>";

								return 0;
						}

						return 1;
				}
		} // end class


?>
