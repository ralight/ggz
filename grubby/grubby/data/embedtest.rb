puts "Hello World."
puts "The message I got is: <" + $answer[0] + ">"
$answer[0].gsub!(/[0-9]/, '***')
puts "Let's return <" + $answer[0] + ">"

