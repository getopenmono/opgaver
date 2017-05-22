#!/usr/bin/env node

// This file modifies the Fromawork makefile to include building the content
// of this directory.

// This script requires you to checkout this repository inside the /src dir in 
// mono_framework.

'use strict'

let fs = require('fs')
let path = require('path')

let relExp = /(MONO_INCLUDES_REL = [\S\s]+?(\s+)[\w/._-]+ ?)\n/gm
let objExp = /(MONO_OBJECTS =[\s\S]+?(\s+)[\w$()%. ,_/*]+)\n/gm

let makefilePath = process.argv[2];

if (!fs.existsSync(makefilePath)) {
	console.error(`Could not find makefile: ${makefilePath}`)
	process.exit(1)
}

function insertIncludeLine(dirName) {
	let relSubst = `$1 \\$2${dirName}\n`
	substitude = substitude.replace(relExp, relSubst)
}

function processIncludes(parentDirname, basename) {
	
	let dirs = fs.readdirSync(parentDirname)
	for (var d in dirs) {
		
		if (dirs[d].match(/^\./) !== null) {
			continue
		}
		
		if (fs.lstatSync(`${parentDirname}/${dirs[d]}`).isDirectory()) {
			console.log(`Adding include: ${basename}/${dirs[d]}`)
			insertIncludeLine(`${basename}/${dirs[d]}`)
		}
	}
}

let makefileContent = fs.readFileSync(makefilePath).toString()
var substitude = makefileContent
let scanDir = process.argv.length > 3 ? process.argv[3] : '.'

var basename;
if (!path.isAbsolute(scanDir)) {
	basename = path.basename( path.normalize( path.join(process.cwd(), scanDir) ) )
}
else {
	basename = path.basename(parentDirname)
}

processIncludes(scanDir, basename)

console.log(`Adding build dependencies  for: ${basename}/*/*.cpp...`)
let objSubst = `$1 \\$2$(patsubst %.cpp,%.o,$(wildcard $(FRAMEWORK_PATH)/${basename}/*/*.cpp))\n`
var substitude = substitude.replace(objExp, objSubst)

fs.writeFileSync(`${makefilePath}.original`, makefileContent)
fs.writeFileSync(`${makefilePath}`, substitude)

process.exit(0)