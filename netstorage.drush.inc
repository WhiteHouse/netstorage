<?php

/**
 * @file
 * Akamai Bulk Upload Cache Clear drush command.
 */

/**
 * Implements hook_drush_command().
 *
 * @return array
 *   An associative array describing your command(s).
 */
function netstorage_drush_command() {
  $items = array();

  $items['netstorage-get-sync-command'] = array(
    'description' => "Prints a command to manually rsync a single file to Akamai's NetStorage.",
    'arguments' => array(
      'filepath' => 'path/to/file to be synced',
    ),
    'examples' => array(
      'drush akamai-netstorage-update "/example/something.pdf"' => 'Update sites/default/files/example/something.pdf on NetStorage',
      'drush anu <filepath>' => '',
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
 * @param string $file
 *  File name
 *
 * @return
 *  String. Commands to execute to manually run NetStorage update for a single
 *  file.
 */
function drush_netstorage_get_sync_command($file) {
  // Get absolute path to file.
  if (!file_exists($file)) {
    drush_print('Sorry. This file does not exist: ' . $file);
    return;
  }
  else {
    $file = realpath($file);
    drush_print("Now \$file is this $file");
  }

  // Make sure variables aren't missing.
  if (!$netstorage_rsync_variables = variable_get('netstorage_rsync_variables', FALSE)) {
    drush_print('Missing netstorage_rsync_variables. See netstorage/README.');
    return;
  }

  $command = _drush_netstorage_get_sync_command($file, $netstorage_rsync_variables);
  drush_print($command);
}

/**
 * Call this for drush commands updating NetStorage for individual files.
 *
 * @param string $filepath
 *  Absolute path to file.
 *
 * @param array $netstorage_rsync_variables
 *    $conf['netstorage_rsync_variables'] = array(
 *      '%key_file' => "/private/path/to/netstorage.key",
 *      '%user' => 'myUserName',
 *      '%customer' => 'myCustomer',
 *      '%cp_code' => 'CPCodeProvidedByAkamai',
 *      '%netstorage_upload_path' => 'htdocs/path/to/your/files/directory'
 *    );
 *
 * @return
 *  String. Commands to execute to manually run NetStorage update for a single
 *  file.
 */
function _drush_netstorage_get_sync_command($filepath, $netstorage_rsync_variables) {
  $rsynch_opts = '-ltgoDzdW --timeout=480';

  $file_info = array(
    '%filename' => $filepath,
    '%rsync_options' => $rsynch_opts,
  );

  $rsync_vars = array_merge($netstorage_rsync_variables, $file_info);

  // Generate the rsync against the file.
  $command = strtr("rsync -e 'ssh -qi %key_file' %rsync_options %files_path%filename %user@%customer.upload.akamai.com:/%cp_code/%netstorage_upload_path/", $rsync_vars);

  return $command;
}
