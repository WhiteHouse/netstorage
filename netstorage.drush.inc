<?php

/**
 * @file
 * Drush commands for the NetStorage module.
 */

/**
 * Implements hook_drush_command().
 */
function netstorage_drush_command() {
  $items = array();

  $items['netstorage-get-scp-command'] = array(
    'description' => "Prints a command to manually rsync a single file to Akamai's NetStorage.",
    'arguments' => array(
      'filepath' => 'path/to/file to be synced',
    ),
    'examples' => array(
      'drush nsc "/example/something.pdf"' => 'Gets the command to update sites/default/files/example/something.pdf on NetStorage.',
    ),
    'aliases' => array('nscp'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function netstorage_drush_help($section) {
  switch ($section) {
    case 'drush:netstorage-get-scp-command':
      return dt("Pass path/to/some/file.example to this script. It will return a command to manually sync this file to NetStorage.");
  }
}

/**
 * Implements drush_hook_COMMAND().
 *
 * @see netstorage_drush_command()
 */
function drush_netstorage_get_scp_command($file) {
  // Get absolute path to file.
  $file = (substr($file, 0, 1) == '/') ? $file : $_ENV['PWD'] . '/' . $file;
  if (!file_exists($file)) {
    drush_print('Sorry. This file does not exist: ' . $file);
    return;
  }
  else {
    $file = realpath($file);
  }

  // @todo If variables are not stored in settings.php, prompt user to input
  //   from command line.
  $netstorage_credentials = variable_get('netstorage_credentials', FALSE);

  $command = _drush_netstorage_get_scp_command($file, $netstorage_credentials);
  drush_print($command);
}

/**
 * Call this for Drush commands updating NetStorage for individual files.
 *
 * scp -i /path/to/key/file /path/to/local/file user@host:/path/to/directory
 *
 * @param string $filepath
 *   The path to the file to be updated.
 *
 * @param array $netstorage_credentials
 *   An array of NetStorage rsync variables containing the follow key-value
 *   pairs:
 *   - "%key_file": The path to your NetStorage key file.
 *   - "%user": Your NetStorage username.
 *   - "%customer": Your NetStorage customer name.
 *   - "%cp_code": Your CP code provided by Akamai.
 *   - "%netstorage_upload_path": The Netstorage upload path.
 *
 * @return string
 *   Commands to execute to manually run NetStorage update for a single file.
 *
 * @see drush_netstorage_get_sync_command()
 */
function _drush_netstorage_get_scp_command($filename, $netstorage_credentials) {
  $variables = array_merge($netstorage_credentials, array('%filename' => $filename));

  // Generate the scp command.
  $command = strtr("scp -i %key_file %filename %user@%customer.upload.akamai.com:/%cp_code/%netstorage_upload_path/", $variables);

  return $command;
}
