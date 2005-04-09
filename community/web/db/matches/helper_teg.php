<?php

function teg_loadmap()
{
	$list = array();

	$doc = domxml_open_file("teg/teg_theme.xml");
	$root = $doc->document_element();
	$nodes = $root->child_nodes();
	foreach ($nodes as $node)
	{
		if ($node->node_name() == "continent") :
			$attrlist = $node->attributes();
			foreach ($attrlist as $attr)
			{
				if ($attr->name == "name") :
					$continent_name = $attr->value;
				elseif ($attr->name == "pos_x") :
					$continent_posx = $attr->value;
				elseif ($attr->name == "pos_y") :
					$continent_posy = $attr->value;
				endif;
			}
			$countries = $node->child_nodes();
			foreach ($countries as $country)
			{
				if ($country->node_name() == "country") :
					$attrlist = $country->attributes();
					foreach ($attrlist as $attr)
					{
						if ($attr->name == "name") :
							$country_name = $attr->value;
						elseif ($attr->name == "pos_x") :
							$country_posx = $attr->value + $continent_posx;
						elseif ($attr->name == "pos_y") :
							$country_posy = $attr->value + $continent_posy;
						elseif ($attr->name == "army_x") :
							$country_armyx = $attr->value;
						elseif ($attr->name == "army_y") :
							$country_armyy = $attr->value;
						elseif ($attr->name == "file") :
							$country_file = $attr->value;
						endif;
					}

					$country = array();
					$country["file"] = $country_file;
					$country["posx"] = $country_posx;
					$country["posy"] = $country_posy;
					$country["armyx"] = $country_armyx;
					$country["armyy"] = $country_armyy;
					$list[$country_name] = $country;
				endif;
			}
		endif;
	}

	return $list;
}

?>
