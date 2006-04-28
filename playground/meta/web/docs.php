<?php include_once("header.inc"); ?>

<div id="content">
	<h4>Documentation</h4>
	<p>
		There is a FAQ available which answers some questions.
	</p>
	<h4>FAQ</h4>
	<p>
		Q: Is there a desktop application which uses MetAggegator?
		<br/>
		A: Of course there is! The web view is only a convenience interface
		which is there to be part of Web 3.0 or whatever the hype is called
		right now. The first real application is <a href="">Metakel</a>,
		a KDE client which lets you launch game clients or a GGZ core client.
		MetAggegator support is also thought to be included with
		XQF and KQF.
		<br/><br/>
		Q: Does MetAggregator waste metaserver bandwidth?
		<br/>
		A: No, it doesn't. In fact, while in the worst case the same bandwidth
		is needed, in the best case it might even save bandwidth since results
		are cached for some seconds. Right now, results are considered valid up
		to 10 seconds for the MetAggregator service, and up to 60 seconds for
		the web interface, which prevents query load should we ever be slashdotted
		(hopefully not!), and in general the web view doesn't need to be as
		up to date as the desktop application's query view, which is often
		followed by a game launch.
		<br/><br/>
		Q: Is there an RSS export available? Planet, a blog feed aggregator,
		provides one!
		<br/>
		A: Sure, there is! Just append /rss to the game name which you want to
		query. Doing this for games which already use an RSS-based metaserver,
		like Freeciv, funnily results in a <a href="/freeciv/rss">copy</a>
		of their metaserver feed.
		<br/><br/>
		Q: I think GGZ are evil and want to take over the world!
		<br/>
		A: Not true. We're not interested in Antarctica at the moment.
		<br/><br/>
		Q: Can I download MetAggegator as true Free Software?
		<br/>
		A: Yes you can. Because we want to <a href="">take over the world</a>... oops!
	</p>

</div>

<?php include_once("footer.inc"); ?>
