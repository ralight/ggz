<?php

class Savegame
{
	var $game;

	function Savegame()
	{
		$this->game = array();
		$this->width = 0;
		$this->height = 0;
	}

	function freelords_loadmap($map)
	{
		$list = array();

		$doc = domxml_open_file($map);
		$root = $doc->document_element();
		$nodes = $root->child_nodes();
		foreach ($nodes as $node)
		{
			if ($node->node_name() == "map") :
				$mapprops = $node->child_nodes();
				foreach ($mapprops as $mapprop)
				{
					if ($mapprop->node_name() == "d_types") :
						$map = $mapprop->get_content();
						$lines = explode("\n", $map);
						for ($j = 0; $j < sizeof($lines); $j++)
							for($i = 0; $i < strlen($lines[$j]); $i++)
								$this->game[$i][$j] = $lines[$j][$i];
					elseif ($mapprop->node_name() == "d_size") :
						$size = $mapprop->get_content();
						$this->width = $size;
						$this->height = $size;
					endif;
				}
			endif;
		}

		return $list;
	}

	function load($savegame)
	{
		$this->freelords_loadmap($savegame);
	}
}

?>
