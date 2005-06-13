<?php

class Savegame
{
	var $game;

	function Savegame()
	{
		$this->game = array();
		$this->stacks = array();
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
			elseif ($node->node_name() == "playerlist") :
				$listprops = $node->child_nodes();
				foreach ($listprops as $listprop)
				{
					if ($listprop->node_name() == "player") :
						$playerprops = $listprop->child_nodes();
						foreach ($playerprops as $playerprop)
						{
							if ($playerprop->node_name() == "stacklist") :
								$stacklistprops = $playerprop->child_nodes();
								foreach ($stacklistprops as $stacklistprop)
								{
									if ($stacklistprop->node_name() == "stack") :
										$stackprops = $stacklistprop->child_nodes();
										$x = 0;
										$y = 0;
										$player = 0;
										foreach ($stackprops as $stackprop)
										{
											$value = $stackprop->get_content();
											if ($stackprop->node_name() == "d_x") :
												$x = $value;
											elseif ($stackprop->node_name() == "d_y") :
												$y = $value;
											elseif ($stackprop->node_name() == "d_player") :
												$player = $value;
											endif;
										}
										$this->stacks[$x][$y] = $player;
									endif;
								}
							endif;
						}
					endif;
				}
			elseif ($node->node_name() == "citylist") :
				$listprops = $node->child_nodes();
				foreach ($listprops as $listprop)
				{
					if ($listprop->node_name() == "city") :
						$cityprops = $listprop->child_nodes();
						$x = 0;
						$y = 0;
						$player = 0;
						foreach ($cityprops as $cityprop)
						{
							$value = $cityprop->get_content();
							if ($cityprop->node_name() == "d_x") :
								$x = $value;
							elseif ($cityprop->node_name() == "d_y") :
								$y = $value;
							elseif ($cityprop->node_name() == "d_owner") :
								$player = $value;
							endif;
						}
						$this->cities[$x][$y] = $player;
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
