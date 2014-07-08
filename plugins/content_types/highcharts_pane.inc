<?php
/**
 * @file
 * Ctools content type plugin file for individual Highcarts pane.
 */

$plugin = array(
"single" => TRUE, // Just do this one, it is needed.
"title" => t("Highcharts Pane"), // Title to show up on the pane screen.
"description" => t("Custom chart"), // Description to show up on the pane screen.
"category" => t("Charts"), // A category to put this under.
"edit form" => "highcharts_pane_edit_form", // A function that will return the settings form for the pane.
"render callback" => "highcharts_pane_render", // A function that will return the renderable content.
"admin info" => "highcharts_pane_info", // A function that will return the information displayed on the admin screen (optional).
"defaults" => array( // Array of defaults for the settings form.
"text" => "",
),
"all contexts" => TRUE, // This is NEEDED to be able to use substitution strings in your pane.
);
 
function highcharts_pane_edit_form($form, &$form_state) {

	form_load_include($form_state, "inc", "highcharts_pane", "plugins/content_types/highcharts_pane");
	$conf = $form_state['conf'];
	$form["chart_title"] = array(
		'#type' => 'textfield',
	    '#title' => t('Chart Title'),
	    '#description' => t("Title of the chart."),
	    "#default_value" => isset($conf["chart_title"]) ? $conf["chart_title"] : '',
	);

	$form["chart_type"] = array(
		"#type" => "select",
		"#title" => t("Chart Type"),
		"#description" => t("The type of chart to render"),
		"#options" => array(
			"Bar" => "Bar",
			"Line" => "Line",
			"Column" => "Column",
			"Pie" => "Pie",
		),
		"#default_value" => isset($conf["chart_type"]) ? $conf["chart_type"] : '',
	);

	$form["subtitle"] = array(
		"#type" => "textfield",
		"#title" => t("Chart Subtitle"),
		"#default_value" => isset($conf["subtitle"]) ? $conf["subtitle"] : '',
	);
	$form["y_axis"] = array(
		"#type" => "textfield",
		"#title" => t("Y-Axis Label"),
		"#description" => t("Label of Y-Axis"),
		"#default_value" => isset($conf["y_axis"]) ? $conf["y_axis"] : '',
	);
	$form["csv"] = array(
		"#type" => "managed_file",
		"#default_value" => isset($conf["csv"]) ? $conf["csv"] : '',
		"#title" => t("Chart Data"),
		"#description" => t("The CSV the chart data will come from. Currently csv is the only allowed extension."),
		"#upload_location" => "public://highcharts",
		"#upload_validators" => array(
			"file_validate_extensions" => array("csv"),
		),

	);

	return $form;
}

// function highcharts_pane_csv_process($element, &$form_state, $form) {
//   $element = file_managed_file_process($element, $form_state, $form);
//   $element['upload_button']['#access'] = FALSE;
//   return $element;
// }

function highcharts_pane_edit_form_submit(&$form, &$form_state) {

	foreach (array("chart_title", "subtitle", "y_axis", "chart_type") as $key) {
		$form_state["conf"][$key] = $form_state["values"][$key];
	}

	if ($form_state["values"]["csv"] != 0) {

		// Load the file via file.fid.
		$file = file_load($form_state['values']['csv']);
		// Change status to permanent.
		$file->status = FILE_STATUS_PERMANENT;
		// Save.
		file_save($file);
		// Record that the module is using the file. 
		file_usage_add($file, 'highcharts_pane', 'user', $file->uid);
		$form_state["conf"]["csv"] = $form_state["values"]["csv"];
		drupal_set_message(t("The file @file_name was uploaded", array("@file_name" => $file->filename,)), 'status', FALSE);
	}
	else if ($form_state["values"]["csv"] == 0) {
		$fid = $form_state["conf"]["csv"];
		$file = $fid ? file_load($fid) : FALSE;
		if ($file) {
			file_usage_delete($file, "highcharts_pane", 'user', $file->uid, 1);

			file_delete($file);
		}
		$form_state["conf"]["csv"] = FALSE;
		drupal_set_message(t("The file @file_name was removed", array("@file_name" => $file->filename)), 'status', FALSE);
	}

}

function highcharts_pane_render($subtype, $conf, $panel_args, $context) {
	
	if (!isset($conf["chart_id"])) {
		$chartID = strtolower($conf["chart_title"]);
		$chartID = preg_replace("/[\s-]+/", " ", $chartID);
	    //Convert whitespaces and underscore to dash
	    $chartID = preg_replace("/[\s_]/", "-", $chartID);

	    $conf["chart_id"] = $chartID;
	} else {
		$chartID = $conf["chart_id"];
	}

	$highcharts_settings = array(
		"chartID" => $chartID,
		"chartData" => _build_chart_data($conf),
	);

	$block = new stdClass();
	$block->title = $conf["chart_title"];
	$block->content ="<div id='" . $chartID . "'></div>";

	libraries_load("highcharts");
	if (($libraries = libraries_load("highcharts")) && ($libraries["loaded"])) {
		drupal_add_js(array("highchartsPane" => $highcharts_settings), "setting");
		drupal_add_js(drupal_get_path("module", "highcharts_pane") . "/js/highcharts_pane.js");
	}

	return $block;
}


function _build_chart_data($options) {
	
	$file = file_load($options["csv"]);
	$file_contents = explode("\n", file_get_contents($file->uri));
	$chart_data = array(
		"chart" => array(
			"renderTo" => $options["chart_id"],
		),
		"title" => array(
			"text" => $options["chart_title"],
		),
	);

	if ($options["chart_type"] != "Pie") {
		$data = _get_normal_chart_data($file_contents);
		$chart_data["chart"]["type"] = strtolower($options["chart_type"]);
		$chart_data["subtitle"] = array(
			"text" => $options["subtitle"],
		);
		$chart_data["xAxis"] = array(
			"categories" => $data["categories"],
		);
		$chart_data["yAxis"] = array(
			"title" => array(
				"text" => $options["y_axis"],
			),
		);
		$chart_data["series"] = $data["series"];
	} else {
		$data = _get_pie_chart_data($file_contents);
		$data["series"]["name"] = $options["y_axis"];
		$chart_data["plotOptions"] = array(
			"allowPointSelect" => TRUE,
			"cursor" => "pointer",
			"dataLabels" => array(
				"enabled" => TRUE,
				"format" => '<b>{point.name}</b>: {point.percentage:.1f} %',
				"style" => array(
					"color" => "black",
				),
			),
		);
		$chart_data["tooltip"] = array(
			"pointFormat" => '{series.name}: <b>{point.percentage:.1f}%</b>',
		);
		$chart_data["series"] = $data["series"];
	}
	return $chart_data;

}

function _get_normal_chart_data($file_contents) {
	$data = array();
	foreach($file_contents as $line_number => $line) {
		$contents = explode(",", $line);

		if ($line_number == 0) {
			$data["categories"] = $contents;
		} else {
			$data["series"][] = array(
				"name" => $contents[0],
				"data" => array_slice(array_map("floatval",$contents), 1),
			);
		}
	}
	return $data;
}

function _get_pie_chart_data($file_contents) {
	$data = array();
	foreach($file_contents as $line) {
		$contents = explode(",", $line);
		$data["series"]["data"][] = array($contents[0], floatval($contents[1]));
	}
	$data["series"]["type"] = "pie";
	return $data;
}