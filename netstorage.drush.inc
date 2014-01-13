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

  $items['netstorage-get-sync-command'] = array(
    'description' => "Prints a command to manually rsync a single file to Akamai's NetStorage.",
    'arguments' => array(
      'filepath' => 'path/to/file to be synced',
    ),
    'examples' => array(
      'drush nsc "/example/something.pdf"' => 'Gets the command to update sites/default/files/example/something.pdf on NetStorage.',
    ),
    'aliases' => array('nsc'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function netstorage_drush_help($section) {
  switch ($section) {
    case 'drush:netstorage-get-sync-command':
      return dt("Pass path/to/some/file.example to this script. It will return a command to manually sync this file to NetStorage.");
  }
}

/**
 * Implements drush_hook_COMMAND().
 *
 * @see netstorage_drush_command()
 */
function drush_netstorage_get_sync_command($file) {
  // @todo Get absolute path to the file here?
  // @todo Do some sort of validation?
  // @todo Throw an error if NetStorage creds aren't available.
  // @todo Throw and error if NetStorage creds aren't valid.
  drush_print('NOTE: This is a work in progress. Probably still requires some testing and debugging. See README for instructions on what NetStorage creds need to be stored in settings.php. Then test out the command to make sure it works.');
  // Get absolute path to file.
  $file = (substr($file, 0, 1) == '/') ? $file : $_ENV['PWD'] . '/' . $file;
  if (!file_exists($file)) {
    drush_print('Sorry. This file does not exist: ' . $file);
    return;
  }
  else {
    $file = realpath($file);
    drush_print("Now \$file is this $file");
  }

  // @todo If variables are not stored in settings.php, prompt user to input
  //   from command line.
  $netstorage_rsync_variables = variable_get('netstorage_rsync_variables', FALSE);

  $command = _drush_netstorage_get_sync_command($file, $netstorage_rsync_variables);
  drush_print($command);
}

/**
 * Call this for Drush commands updating NetStorage for individual files.
 *
 * @param string $filepath
 *   The path to the file to be updated.
 *
 * @param array $netstorage_rsync_variables
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
function _drush_netstorage_get_sync_command($filepath, $netstorage_rsync_variables) {

  $filename = '@todo';
  $rsynch_opts = '-ltgoDzdW --timeout=480';
  $files_path = '@todo';

  $file_info = array(
    '%filename' => $filename,
    '%files_path' => $files_path,
    '%rsync_options' => $rsynch_opts,
  );

  $rsync_vars = array_merge($netstorage_rsync_variables, $file_info);

  // Generate the rsync against the file.
  $command = strtr("rsync -e 'ssh -qi %key_file' %rsync_options %filename %user@%customer.upload.akamai.com:/%cp_code/%netstorage_upload_path/", $rsync_vars);

  return $command;
}
