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
    'options' => array(
      'subdir' => 'Subdirectory on NetStorage to push file into' 
    ),
    'examples' => array(
      'drush netstorage-get-scp-command "something.pdf"' => 'If you set %netstorage_upload_path to "htdocs/sites/default/files" in settings.php this command will push your file up to htdocs/sites/default/files/something.pdf on NetStorage.',
      'drush ngscp "/local/path/to/something.pdf --subdir=example"' => 'Gets the command to update %netstorage_upload_path/example/something.pdf on NetStorage.',
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

  // Get subdir (if one is provided).
  $subdir = drush_get_option('subdir', '');

  $command = netstorage_get_scp_command($file, $subdir);
  drush_print($command);
}
