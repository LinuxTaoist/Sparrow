/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIRender.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : HTML/CSS/JS rendering assets for HTTP UI
 *  @date       : 2026/05/28
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2026/05/28 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __HUI_RENDER_H__
#define __HUI_RENDER_H__

#include <string>

namespace HUIRender {

static const std::string HTML_TEMPLATE =
R"__HUI__(<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Sparrow Device Control Center</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{
	--bg-1:#f4f7fb;
	--bg-2:#e8f1ec;
	--ink:#17202b;
	--muted:#607082;
	--panel:#ffffff;
	--line:#dce5ee;
	--brand:#0f766e;
	--brand-strong:#115e59;
	--ok:#2e7d32;
	--warn:#ed6c02;
	--danger:#c62828;
	--shadow:0 10px 28px rgba(15,23,42,.06);
}
html,body{width:100%;height:100%;font-family:'Segoe UI',Roboto,sans-serif;color:var(--ink);background:radial-gradient(circle at 12% 10%,#e4f6ff 0,var(--bg-1) 36%,var(--bg-2) 100%)}
body{overflow-y:auto}
.shell{min-height:100vh;padding:22px 24px 28px}
.topbar{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:18px}
.title-wrap h1{font-size:30px;letter-spacing:.2px}
.subtitle{margin-top:6px;color:var(--muted);font-size:14px;max-width:720px}
.nav{display:flex;gap:10px;flex-wrap:wrap;margin-top:16px}
.nav-link{padding:9px 14px;border-radius:999px;border:1px solid var(--line);background:#fff;color:#304254;text-decoration:none;font-size:13px;font-weight:600;transition:all .2s ease}
.nav-link.active{background:var(--brand);border-color:var(--brand);color:#fff;box-shadow:0 8px 20px rgba(15,118,110,.22)}
.nav-link:hover{border-color:var(--brand);color:var(--brand)}
.chips{display:flex;flex-wrap:wrap;justify-content:flex-end;gap:10px}
.chip{display:flex;align-items:center;gap:8px;padding:7px 12px;border-radius:999px;background:#fff;border:1px solid var(--line);font-size:13px;color:#334155}
.dot{width:9px;height:9px;border-radius:50%;background:#94a3b8;transition:all .2s ease}
.dot.ok{background:var(--ok);box-shadow:0 0 0 4px rgba(46,125,50,.15)}
.dot.warn{background:var(--warn);box-shadow:0 0 0 4px rgba(237,108,2,.12)}
.dot.err{background:var(--danger);box-shadow:0 0 0 4px rgba(198,40,40,.15)}
.chip.subtle-note{font-size:12px;color:#8a6b21;background:#fff8e8;border-color:#f2e4be;opacity:.85;display:none}
.chip.subtle-note.show{display:flex}
.page{display:none}
.page.active{display:block}
.grid{display:grid;grid-template-columns:repeat(12,minmax(0,1fr));gap:14px}
.card{background:var(--panel);border:1px solid var(--line);border-radius:16px;box-shadow:var(--shadow)}
.metric{grid-column:span 3;padding:16px 18px}
.metric .label{font-size:12px;color:var(--muted);text-transform:uppercase;letter-spacing:.7px}
.metric .value{margin-top:8px;font-size:34px;font-weight:700;line-height:1}
.metric .hint{margin-top:8px;font-size:12px;color:#64748b}
.health{grid-column:span 8;padding:16px 18px}
.health-head{display:flex;justify-content:space-between;align-items:center;margin-bottom:14px}
.health-title{font-size:15px;font-weight:600}
.health-tag{font-size:12px;padding:4px 9px;border-radius:999px;color:#fff;background:var(--ok)}
.health-bar{height:10px;border-radius:999px;background:#eef2f7;overflow:hidden}
.health-fill{height:100%;width:0;background:linear-gradient(90deg,var(--brand),#22c55e);transition:width .25s ease}
.health-note{margin-top:10px;font-size:13px;color:#64748b}
.feed{grid-column:span 4;padding:16px 18px}
.sec-title{font-size:15px;font-weight:600;margin-bottom:12px}
.feed-list{list-style:none;display:flex;flex-direction:column;gap:8px;max-height:180px;overflow:auto;padding-right:2px}
.feed-item{display:flex;justify-content:space-between;align-items:center;padding:9px 10px;background:#f8fbff;border:1px solid #e9eff7;border-radius:8px;font-size:12px;color:#334155}
.feed-item b{font-size:12px;color:#0f172a}
.split-card{grid-column:span 6;padding:0;overflow:hidden;min-height:408px;display:flex;flex-direction:column}
.panel-head{display:flex;justify-content:space-between;align-items:center;padding:14px 18px;background:#f8fbff;border-bottom:1px solid var(--line)}
.panel-note{font-size:12px;color:#64748b}
.device-body{padding:16px 18px;display:grid;grid-template-columns:1fr 1fr;gap:12px;align-content:start;flex:1}
.device-cell{padding:14px;border:1px solid #e8eef6;border-radius:12px;background:linear-gradient(180deg,#fbfdff 0,#f6fafc 100%)}
.device-cell b{display:block;margin-bottom:8px;font-size:12px;text-transform:uppercase;letter-spacing:.6px;color:var(--muted)}
.device-cell span{display:block;font-size:15px;color:#1e293b;line-height:1.4;word-break:break-word}
.table-shell{padding:0 12px 12px;flex:1;overflow:hidden}
.table-wrap{max-height:368px;overflow:auto;border:1px solid #e8eef6;border-radius:12px}
table{width:100%;border-collapse:collapse;background:#fff}
thead th{position:sticky;top:0;z-index:1;padding:12px 14px;background:#fbfdff;text-align:left;font-size:12px;text-transform:uppercase;letter-spacing:.6px;color:#64748b;border-bottom:1px solid var(--line)}
tbody td{padding:12px 14px;font-size:13px;color:#334155;border-bottom:1px solid #edf2f7;white-space:nowrap}
tbody tr:hover{background:#f8fbff}
.empty{text-align:center;color:#94a3b8;padding:18px}
.terminal-layout{display:grid;grid-template-columns:minmax(0,1fr) 300px;gap:14px}
.terminal-card{padding:0;overflow:hidden}
.terminal-body{padding:16px}
.shell-log{background:#18222f;color:#d7efe8;padding:14px;border-radius:12px;font-family:monospace;font-size:12px;max-height:calc(100vh - 320px);overflow-y:auto;min-height:420px;border:1px solid #243446;margin-bottom:12px;white-space:pre-wrap;word-wrap:break-word;line-height:1.45}
.shell-prompt{color:#d7efe8}
.shell-userhost{color:#7dd3fc;font-weight:600}
.shell-path{color:#86efac;font-weight:600}
.shell-sign{color:#facc15;font-weight:700}
.shell-command{color:#f8fafc}
.shell-output{color:#d7efe8}
.terminal-input-row{display:flex;gap:8px}
.terminal-input{flex:1;padding:10px 12px;border:1px solid #294154;border-radius:10px;font-family:monospace;font-size:12px;background:#18222f;color:#ecfeff}
.terminal-btn{padding:10px 14px;background:var(--brand);color:#fff;border:none;border-radius:10px;cursor:pointer;font-size:12px;font-weight:600}
.terminal-btn:hover{background:var(--brand-strong)}
.terminal-btn.secondary{background:#475569}
.terminal-btn.secondary:hover{background:#334155}
.guide-card{padding:16px 18px}
.guide-list{list-style:none;display:flex;flex-direction:column;gap:12px}
.guide-item{padding:12px;border:1px solid #e8eef6;border-radius:12px;background:#fbfdff}
.guide-item b{display:block;margin-bottom:6px;font-size:12px;text-transform:uppercase;letter-spacing:.6px;color:var(--muted)}
.guide-item span{display:block;font-size:13px;line-height:1.5;color:#334155}
@media (max-width:1200px){.metric{grid-column:span 6}.health{grid-column:span 12}.feed{grid-column:span 12}.split-card{grid-column:span 12}.terminal-layout{grid-template-columns:1fr}.chips{justify-content:flex-start}}
@media (max-width:720px){.shell{padding:14px}.topbar{flex-direction:column}.device-body{grid-template-columns:1fr}.metric{grid-column:span 12}.table-wrap{max-height:420px}.shell-log{min-height:320px;max-height:none}}
</style>
</head>
<body>
<div class='shell'>
	<div class='topbar'>
		<div class='title-wrap'>
			<h1 id='page-title'>Sparrow Device Control Center</h1>
			<div id='page-subtitle' class='subtitle'>Overview and terminal access for the current device.</div>
			<div class='nav'>
				<a id='nav-dashboard' class='nav-link' href='/'>Device Uplink</a>
				<a id='nav-terminal' class='nav-link' href='/terminal'>Remote Terminal</a>
			</div>
		</div>
		<div class='chips'>
			<div class='chip'><span id='conn-dot' class='dot'></span><span id='conn-text'>Connecting</span></div>
			<div class='chip'>Host: <span id='chip-host'>-</span></div>
			<div class='chip'>Last Refresh: <span id='last-refresh'>-</span></div>
			<div class='chip'>Latency: <span id='req-latency'>-</span></div>
			<div id='data-note' class='chip subtle-note'>Showing last valid snapshot</div>
		</div>
	</div>

	<section id='page-dashboard' class='page'>
		<div class='grid'>
			<div class='card metric'>
				<div class='label'>Queue Count</div>
				<div class='value' id='stat-queues'>-</div>
				<div class='hint'>Active message queues</div>
			</div>
			<div class='card metric'>
				<div class='label'>Pending Messages</div>
				<div class='value' id='stat-pending'>-</div>
				<div class='hint'>Current backlog</div>
			</div>
			<div class='card metric'>
				<div class='label'>Total Messages</div>
				<div class='value' id='stat-total'>-</div>
				<div class='hint'>Cumulative traffic</div>
			</div>
			<div class='card metric'>
				<div class='label'>Peak Usage</div>
				<div class='value' id='stat-peak'>-</div>
				<div class='hint'>Highest queue usage</div>
			</div>

			<div class='card health'>
				<div class='health-head'>
					<div class='health-title'>Device Health</div>
					<div id='health-tag' class='health-tag'>GOOD</div>
				</div>
				<div class='health-bar'><div id='health-fill' class='health-fill'></div></div>
				<div class='health-note'>Busiest Queue: <b id='busiest-queue'>-</b></div>
			</div>

			<div class='card feed'>
				<div class='sec-title'>Realtime Feed</div>
				<ul id='feed-list' class='feed-list'>
					<li class='feed-item'><span>Waiting for data</span><b>--</b></li>
				</ul>
			</div>

			<div class='card split-card'>
				<div class='panel-head'>
					<div class='sec-title' style='margin:0'>Device Profile</div>
					<div class='panel-note'>Base uplink information</div>
				</div>
				<div class='device-body'>
					<div class='device-cell'><b>Hostname</b><span id='profile-hostname'>-</span></div>
					<div class='device-cell'><b>Uptime</b><span id='profile-uptime'>-</span></div>
					<div class='device-cell'><b>Kernel</b><span id='profile-kernel'>-</span></div>
					<div class='device-cell'><b>Memory / Disk</b><span id='profile-resources-value'>-</span></div>
				</div>
			</div>

			<div class='card split-card'>
				<div class='panel-head'>
					<div class='sec-title' style='margin:0'>Queue Snapshot (sub-module)</div>
					<div class='panel-note'>8 rows per view, scroll for more</div>
				</div>
				<div class='table-shell'>
					<div class='table-wrap'>
						<table>
							<thead>
								<tr><th>Name</th><th>Handle</th><th>Pending</th><th>Total</th><th>Peak</th></tr>
							</thead>
							<tbody id='queue-table'>
								<tr><td colspan='5' class='empty'>Loading...</td></tr>
							</tbody>
						</table>
					</div>
				</div>
			</div>
		</div>
	</section>

	<section id='page-terminal' class='page'>
		<div class='terminal-layout'>
			<div class='card terminal-card'>
				<div class='panel-head'>
					<div class='sec-title' style='margin:0'>Remote Terminal</div>
					<div class='panel-note'>Interactive shell similar to remote SSH</div>
				</div>
				<div class='terminal-body'>
					<div id='shell-log' class='shell-log'></div>
					<div class='terminal-input-row'>
						<input type='text' id='shell-input' class='terminal-input' placeholder='Enter command...'>
						<button id='shell-exec-btn' class='terminal-btn' onclick='executeShellCmd()'>Send</button>
						<button class='terminal-btn secondary' onclick='interruptShell()'>Ctrl+C</button>
					</div>
				</div>
			</div>
			<div class='card guide-card'>
				<div class='sec-title'>Session Notes</div>
				<ul class='guide-list'>
					<li class='guide-item'><b>Mode</b><span>Commands run in one persistent PTY session, so directory state and long-running output stay alive.</span></li>
					<li class='guide-item'><b>History</b><span>The terminal keeps the current page session log so you can review earlier output.</span></li>
					<li class='guide-item'><b>Tips</b><span>Use Ctrl+C for commands like tail -f, and output will keep streaming without waiting for the command to finish.</span></li>
				</ul>
			</div>
		</div>
	</section>
</div>

<script>
const HUI_ACTIVE_PAGE = '__ACTIVE_PAGE__';
const HUI_REFRESH_INTERVAL_MS = 2000;
const HUI_FEED_LIMIT = 12;
const feedHistory = [];
let hasValidSnapshot = false;
let dashboardTimer = null;
let shellPrompt = '';
let shellReadBusy = false;
let shellPollTimer = null;
let shellConnected = false;
let shellInitBusy = false;
let shellReconnectTimer = null;
let shellInputBound = false;
let shellDisconnectNoted = false;
let shellProgramMode = '';
let shellProgramPrompt = '';
let shellLastSubmittedCommand = '';

function setConn(state, latency) {
	const dot = document.getElementById('conn-dot');
	const txt = document.getElementById('conn-text');
	dot.classList.remove('ok');
	dot.classList.remove('warn');
	dot.classList.remove('err');
	if (state === 'online') {
		dot.classList.add('ok');
		txt.textContent = 'Online';
	} else if (state === 'stale') {
		dot.classList.add('warn');
		txt.textContent = 'Stale';
	} else {
		dot.classList.add('err');
		txt.textContent = 'Offline';
	}
	if (typeof latency === 'number') {
		document.getElementById('req-latency').textContent = latency + ' ms';
	}
}

function markRefresh(value) {
	document.getElementById('last-refresh').textContent = value || '-';
}

function setDataNote(message) {
	const note = document.getElementById('data-note');
	if (message) {
		note.textContent = message;
		note.classList.add('show');
	} else {
		note.textContent = 'Showing last valid snapshot';
		note.classList.remove('show');
	}
}

function setupPage() {
	const dashboardPage = document.getElementById('page-dashboard');
	const terminalPage = document.getElementById('page-terminal');
	const dashboardNav = document.getElementById('nav-dashboard');
	const terminalNav = document.getElementById('nav-terminal');
	const pageTitle = document.getElementById('page-title');
	const pageSubtitle = document.getElementById('page-subtitle');

	if (HUI_ACTIVE_PAGE === 'terminal') {
		terminalPage.classList.add('active');
		terminalNav.classList.add('active');
		pageTitle.textContent = 'Remote Terminal';
		pageSubtitle.textContent = 'Dedicated shell access with a separate page, so monitoring and command execution do not compete for space.';
		setDataNote('');
	} else {
		dashboardPage.classList.add('active');
		dashboardNav.classList.add('active');
		pageTitle.textContent = 'Device Uplink Overview';
		pageSubtitle.textContent = 'Device base data, queue status, and system profile on one readable overview page.';
	}
}

function calcHealth(status, latencyMs) {
	let score = 100;
	const pending = Number(status.pending || 0);
	if (pending > 100) score -= 35;
	else if (pending > 20) score -= 20;
	else if (pending > 0) score -= 10;

	if (latencyMs > 1500) score -= 35;
	else if (latencyMs > 800) score -= 20;
	else if (latencyMs > 400) score -= 10;

	if (score < 0) score = 0;
	return score;
}

function applyHealth(score) {
	const fill = document.getElementById('health-fill');
	const tag = document.getElementById('health-tag');
	fill.style.width = score + '%';
	if (score >= 80) {
		tag.textContent = 'GOOD';
		tag.style.background = '#2e7d32';
	} else if (score >= 50) {
		tag.textContent = 'WARN';
		tag.style.background = '#ed6c02';
	} else {
		tag.textContent = 'CRITICAL';
		tag.style.background = '#c62828';
	}
}

function updateQueueTable(queues) {
	const tb = document.getElementById('queue-table');
	if (!Array.isArray(queues) || queues.length === 0) {
		tb.innerHTML = '<tr><td colspan="5" class="empty">No queue data</td></tr>';
		return;
	}

	tb.innerHTML = queues.map(function (x) {
		return '<tr>'
			+ '<td>' + x.name + '</td>'
			+ '<td>' + x.handle + '</td>'
			+ '<td>' + x.pending + '</td>'
			+ '<td>' + x.total + '</td>'
			+ '<td>' + x.peak + '</td>'
			+ '</tr>';
	}).join('');
}

function pushFeed(status, latencyMs) {
	const item = {
		time: new Date().toLocaleTimeString(),
		pending: status.pending,
		total: status.total,
		latency: latencyMs
	};
	feedHistory.unshift(item);
	if (feedHistory.length > HUI_FEED_LIMIT) {
		feedHistory.pop();
	}

	const list = document.getElementById('feed-list');
	list.innerHTML = feedHistory.map(function (x) {
		return '<li class="feed-item">'
			+ '<span>' + x.time + ' | pending: ' + x.pending + ' | total: ' + x.total + '</span>'
			+ '<b>' + x.latency + ' ms</b>'
			+ '</li>';
	}).join('');
}

function renderDashboard(status, queues, profile, resources, latency, refreshText) {
	document.getElementById('stat-queues').textContent = status.queues;
	document.getElementById('stat-pending').textContent = status.pending;
	document.getElementById('stat-total').textContent = status.total;
	document.getElementById('stat-peak').textContent = status.peak;
	document.getElementById('busiest-queue').textContent = status.busiest || '-';

	if (profile) {
		document.getElementById('profile-hostname').textContent = profile.hostname || '-';
		document.getElementById('profile-uptime').textContent = profile.uptime || '-';
		document.getElementById('profile-kernel').textContent = profile.kernel || '-';
	}

	if (resources) {
		document.getElementById('profile-resources-value').textContent = (resources.memory || '-') + ' / ' + (resources.disk || '-');
	}

	applyHealth(calcHealth(status, latency));
	updateQueueTable(queues);
	pushFeed(status, latency);
	setConn('online', latency);
	markRefresh(refreshText);
	setDataNote('');
}

function loadData() {
	const begin = Date.now();
	Promise.all([
		fetch('/api/status').then(function (r) { return r.json(); }),
		fetch('/api/queues').then(function (r) { return r.json(); }),
		fetch('/api/profile').then(function (r) { return r.json(); }),
		fetch('/api/resources').then(function (r) { return r.json(); })
	]).then(function (arr) {
		const status = arr[0];
		const queues = arr[1];
		const profile = arr[2];
		const resources = arr[3];
		const latency = Date.now() - begin;
		const refreshText = new Date().toLocaleTimeString();
		renderDashboard(status, queues, profile, resources, latency, refreshText);
		hasValidSnapshot = true;
	}).catch(function (e) {
		console.error('Load data error:', e);
		if (hasValidSnapshot) {
			setConn('stale');
			setDataNote('Data delayed, showing last valid snapshot');
			return;
		}

		setConn('offline', Date.now() - begin);
		setDataNote('Waiting for valid data');
		document.getElementById('queue-table').innerHTML = '<tr><td colspan="5" class="empty">Load failed</td></tr>';
	});
}

function escapeHtml(text) {
	return String(text || '')
		.replace(/&/g, '&amp;')
		.replace(/</g, '&lt;')
		.replace(/>/g, '&gt;')
		.replace(/"/g, '&quot;')
		.replace(/'/g, '&#39;');
}

function renderPrompt(prompt) {
	const rawPrompt = String(prompt || '');
	const trimmedPrompt = rawPrompt.replace(/\s+$/, '');
	const match = trimmedPrompt.match(/^([^:]+)(:.*?)([$#])$/);
	if (!match) {
		return '<span class="shell-prompt">' + escapeHtml(rawPrompt) + '</span>';
	}

	return '<span class="shell-prompt">'
		+ '<span class="shell-userhost">' + escapeHtml(match[1]) + '</span>'
		+ '<span class="shell-path">' + escapeHtml(match[2]) + '</span>'
		+ '<span class="shell-sign">' + escapeHtml(match[3]) + '</span>'
		+ ' </span>';
}

function renderOutput(text) {
	return '<span class="shell-output">'
		+ escapeHtml(text || '').replace(/\n/g, '<br>')
		+ '</span>';
}

function appendShellHtml(html) {
	const log = document.getElementById('shell-log');
	log.innerHTML += html;
	log.scrollTop = log.scrollHeight;
}

function appendShellPrompt(prompt) {
	if (!prompt) {
		return;
	}
	appendShellHtml(renderPrompt(prompt));
}

function appendProgramPrompt(prompt) {
	if (!prompt) {
		return;
	}
	appendShellHtml('<span class="shell-output">' + escapeHtml(prompt) + '</span>');
}

function isGdbStartCommand(cmd) {
	return /^\s*gdb(\s|$)/.test(String(cmd || ''));
}

function isGdbQuitCommand(cmd) {
	return /^\s*(quit|q)\s*$/.test(String(cmd || ''));
}

function isGdbResumeCommand(cmd) {
	return /^\s*(run|r|continue|c|start|next|n|step|s|si|ni|finish|until)\b/.test(String(cmd || ''));
}

function shouldSynthesizeGdbPrompt(output) {
	const text = String(output || '');
	const cmd = String(shellLastSubmittedCommand || '');
	if (!text || text.indexOf('(gdb)') >= 0) {
		return false;
	}

	if (isGdbStartCommand(cmd)) {
		return /Reading symbols from /.test(text);
	}

	if (isGdbQuitCommand(cmd)) {
		return false;
	}

	if (isGdbResumeCommand(cmd)) {
		return /\[Inferior .* exited normally\]|Breakpoint \d+,|Temporary breakpoint \d+,|Program received signal|The program is not being run\.|The program being debugged has been started already\./.test(text);
	}

	return text.endsWith('\n');
}

function shellFetchJson(url) {
	return fetch(url, { cache: 'no-store' })
		.then(function (r) {
			if (!r.ok) {
				throw new Error('HTTP ' + r.status);
			}
			return r.json();
		});
}

function stopShellPolling() {
	if (shellPollTimer) {
		clearInterval(shellPollTimer);
		shellPollTimer = null;
	}
	shellReadBusy = false;
}

function scheduleShellReconnect() {
	if (shellReconnectTimer) {
		return;
	}

	shellReconnectTimer = setTimeout(function () {
		shellReconnectTimer = null;
		initShellTerminal(false);
	}, 1000);
}

function handleShellDisconnect(message) {
	shellConnected = false;
	stopShellPolling();
	shellProgramMode = '';
	shellProgramPrompt = '';
	shellLastSubmittedCommand = '';
	setConn('offline');
	markRefresh(new Date().toLocaleTimeString());
	if (message && !shellDisconnectNoted) {
		appendShellHtml(renderOutput(message) + '<br>');
		shellDisconnectNoted = true;
	}
	scheduleShellReconnect();
}

function pollShellOutput() {
	if (shellReadBusy || !shellConnected) {
		return;
	}

	shellReadBusy = true;
	shellFetchJson('/api/shell/read')
		.then(function (data) {
			if (data.output) {
				appendShellHtml(renderOutput(data.output));
				if (shellProgramMode === 'gdb' && shouldSynthesizeGdbPrompt(data.output)) {
					appendProgramPrompt(shellProgramPrompt);
				}
			}

			if (data.promptReady && data.promptAfter) {
				shellProgramMode = '';
				shellProgramPrompt = '';
				shellLastSubmittedCommand = '';
				shellPrompt = data.promptAfter;
				appendShellPrompt(shellPrompt);
			}
		})
		.catch(function () {
			handleShellDisconnect('Terminal disconnected. Reconnecting...');
		})
		.finally(function () {
			shellReadBusy = false;
		});
}

function executeShellCmd() {
	const input = document.getElementById('shell-input');
	const rawCmd = input.value;
	const cmd = rawCmd.trim();
	if (!cmd) {
		return;
	}
	if (!shellConnected) {
		handleShellDisconnect('Terminal offline. Reconnecting...');
		input.focus();
		return;
	}

	input.value = '';
	input.focus();

	shellFetchJson('/api/shell/write?cmd=' + encodeURIComponent(cmd))
		.then(function (data) {
			if (!data.accepted) {
				appendShellHtml(renderOutput('Command send failed') + '<br>');
				return;
			}
			shellLastSubmittedCommand = cmd;
			if (isGdbStartCommand(cmd)) {
				shellProgramMode = 'gdb';
				shellProgramPrompt = '(gdb) ';
			}
			shellPrompt = '';
			appendShellHtml('<span class="shell-command">' + escapeHtml(cmd) + '</span><br>');
			pollShellOutput();
		})
		.catch(function () {
			input.value = cmd;
			handleShellDisconnect('Terminal offline. Reconnecting...');
			input.focus();
		});
}

function interruptShell() {
	if (!shellConnected) {
		handleShellDisconnect('Terminal offline. Reconnecting...');
		return;
	}

	shellFetchJson('/api/shell/interrupt')
		.then(function (data) {
			if (data.ok) {
				appendShellHtml(renderOutput('^C') + '<br>');
				pollShellOutput();
			}
		})
		.catch(function () {
			handleShellDisconnect('Terminal offline. Reconnecting...');
		});
}

function initShellTerminal(resetView) {
	if (shellInitBusy) {
		return;
	}

	shellInitBusy = true;
	shellFetchJson('/api/shell/init')
		.then(function (data) {
			const firstReady = !shellConnected;
			shellPrompt = data.prompt || '';
			shellConnected = true;
			shellDisconnectNoted = false;
			if (resetView) {
				document.getElementById('shell-log').innerHTML = renderOutput('Welcome to Remote Terminal') + '<br><br>' + renderPrompt(shellPrompt);
			} else if (firstReady) {
				appendShellHtml(renderOutput('Terminal reconnected') + '<br>');
				appendShellPrompt(shellPrompt);
			}
			setConn('online', 0);
			markRefresh(new Date().toLocaleTimeString());
			if (!shellPollTimer) {
				shellPollTimer = setInterval(pollShellOutput, 250);
			}
		})
		.catch(function () {
			handleShellDisconnect(resetView ? 'Terminal offline. Waiting to reconnect...' : '');
		})
		.finally(function () {
			shellInitBusy = false;
		});

	if (!shellInputBound) {
		document.getElementById('shell-input').addEventListener('keydown', function (e) {
			if (e.key === 'Enter') {
				executeShellCmd();
			}
		});
		shellInputBound = true;
	}

	document.getElementById('shell-input').focus();
}

document.getElementById('chip-host').textContent = window.location.host || '-';
setupPage();
if (HUI_ACTIVE_PAGE == 'terminal') {
	initShellTerminal(true);
} else {
	loadData();
	dashboardTimer = setInterval(loadData, HUI_REFRESH_INTERVAL_MS);
}
</script>
</body>
</html>)__HUI__";

inline void ReplaceToken(std::string& text, const std::string& token, const std::string& value)
{
	std::string::size_type pos = 0;
	while ((pos = text.find(token, pos)) != std::string::npos) {
		text.replace(pos, token.length(), value);
		pos += value.length();
	}
}

inline std::string BuildHtmlPage(const std::string& activePage)
{
	std::string html = HTML_TEMPLATE;
	ReplaceToken(html, "__ACTIVE_PAGE__", activePage == "terminal" ? "terminal" : "dashboard");
	return html;
}

}

#endif // __HUI_RENDER_H__
