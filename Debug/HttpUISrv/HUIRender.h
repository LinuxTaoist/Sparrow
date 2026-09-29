/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : HUIRender.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : HTML/CSS/JS rendering assets for HTTP UI
 *  @date       : 2026/05/28
 *---------------------------------------------------------------------------------------------------------------------
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
	--bg-1:#f7f9fc;
	--bg-2:#eef4f8;
	--bg-3:#e7eef5;
	--ink:#0f1722;
	--muted:#66768a;
	--muted-soft:#8a98a9;
	--panel:rgba(255,255,255,.72);
	--panel-strong:rgba(255,255,255,.88);
	--line:rgba(148,163,184,.18);
	--line-strong:rgba(148,163,184,.28);
	--brand:#10253f;
	--brand-soft:#36506f;
	--ok:#1f8f67;
	--warn:#d18c1d;
	--danger:#cf5c4b;
	--glow:rgba(117,138,164,.16);
	--shadow:0 24px 60px rgba(15,23,42,.08);
	--shadow-soft:0 10px 24px rgba(15,23,42,.04);
}
html,body{width:100%;min-height:100%;font-family:'Avenir Next','Helvetica Neue','PingFang SC','Noto Sans SC',sans-serif;color:var(--ink)}
html{background:var(--bg-2)}
body{overflow-y:auto;position:relative;background:transparent}
body::before{content:'';position:fixed;inset:0;z-index:-2;background:
	radial-gradient(circle at 14% 10%,rgba(255,255,255,.95) 0,rgba(255,255,255,.72) 20%,transparent 46%),
	radial-gradient(circle at 85% 16%,rgba(210,227,244,.62) 0,transparent 30%),
	radial-gradient(circle at 72% 86%,rgba(220,233,241,.72) 0,transparent 34%),
	linear-gradient(160deg,var(--bg-1) 0,var(--bg-2) 52%,var(--bg-3) 100%);background-repeat:no-repeat;background-size:cover;background-attachment:fixed;transform:translateZ(0)}
body::after{content:'';position:fixed;inset:0;z-index:-1;background:linear-gradient(180deg,rgba(255,255,255,.12) 0,rgba(255,255,255,0) 24%,rgba(230,238,246,.12) 100%);pointer-events:none}
.shell{position:relative;z-index:1;min-height:100vh;max-width:1560px;margin:0 auto;padding:34px 34px 44px}
.topbar{display:flex;justify-content:space-between;align-items:flex-start;gap:28px;margin-bottom:28px}
.title-wrap h1{font-size:40px;font-weight:600;letter-spacing:-.04em;line-height:1.02}
.subtitle{margin-top:10px;color:var(--muted);font-size:15px;line-height:1.7;max-width:760px}
.nav{display:flex;gap:12px;flex-wrap:wrap;margin-top:24px}
.nav-link{padding:11px 18px;border-radius:999px;border:1px solid var(--line);background:rgba(255,255,255,.45);backdrop-filter:blur(14px);-webkit-backdrop-filter:blur(14px);color:#304254;text-decoration:none;font-size:12px;font-weight:600;letter-spacing:.08em;text-transform:uppercase;transition:all .24s ease}
.nav-link.active{background:var(--brand);border-color:transparent;color:#fff;box-shadow:0 14px 34px rgba(16,37,63,.18)}
.nav-link:hover{border-color:var(--line-strong);background:rgba(255,255,255,.78);color:var(--brand)}
.chips{display:flex;flex-wrap:wrap;justify-content:flex-end;gap:12px;max-width:540px}
.chip{display:flex;align-items:center;gap:8px;padding:10px 14px;border-radius:999px;background:rgba(255,255,255,.5);backdrop-filter:blur(18px);-webkit-backdrop-filter:blur(18px);border:1px solid var(--line);font-size:12px;color:#334155;letter-spacing:.02em;box-shadow:var(--shadow-soft)}
.dot{width:9px;height:9px;border-radius:50%;background:#94a3b8;transition:all .2s ease}
.dot.ok{background:var(--ok);box-shadow:0 0 0 5px rgba(31,143,103,.12)}
.dot.warn{background:var(--warn);box-shadow:0 0 0 5px rgba(209,140,29,.12)}
.dot.err{background:var(--danger);box-shadow:0 0 0 5px rgba(207,92,75,.12)}
.chip.subtle-note{font-size:12px;color:#8a6b21;background:rgba(255,248,232,.92);border-color:#f2e4be;opacity:.92;display:none}
.chip.subtle-note.show{display:flex}
.page{display:none}
.page.active{display:block}
.grid{display:grid;grid-template-columns:repeat(12,minmax(0,1fr));gap:18px;align-items:stretch}
.card{background:var(--panel);backdrop-filter:blur(24px);-webkit-backdrop-filter:blur(24px);border:1px solid var(--line);border-radius:28px;box-shadow:var(--shadow);position:relative;overflow:hidden}
.card::before{content:'';position:absolute;inset:0;background:linear-gradient(180deg,rgba(255,255,255,.34),rgba(255,255,255,0));pointer-events:none}
.metric{grid-column:span 3;padding:22px 24px 24px;min-height:180px}
.metric .label{font-size:11px;color:var(--muted);text-transform:uppercase;letter-spacing:.16em}
.metric .value{margin-top:18px;font-size:48px;font-weight:600;letter-spacing:-.05em;line-height:.94}
.metric .hint{margin-top:18px;font-size:13px;color:var(--muted-soft);line-height:1.55}
.health{grid-column:span 8;padding:24px 26px;min-height:180px}
.health-head{display:flex;justify-content:space-between;align-items:center;margin-bottom:18px}
.health-title{font-size:18px;font-weight:600;letter-spacing:-.02em}
.health-tag{font-size:11px;padding:6px 11px;border-radius:999px;color:#fff;background:var(--ok);letter-spacing:.12em;text-transform:uppercase}
.health-bar{height:12px;border-radius:999px;background:rgba(226,232,240,.74);overflow:hidden}
.health-fill{height:100%;width:0;background:linear-gradient(90deg,#223b5a 0,#3f6f8f 52%,#8fd0c2 100%);transition:width .25s ease}
.health-note{margin-top:18px;font-size:13px;color:var(--muted);line-height:1.6}
.feed{grid-column:span 4;padding:24px 24px 22px;height:clamp(280px,34vh,360px);min-height:clamp(280px,34vh,360px);display:flex;flex-direction:column}
.sec-title{font-size:18px;font-weight:600;letter-spacing:-.02em;margin-bottom:12px}
.feed-list{list-style:none;display:flex;flex-direction:column;gap:10px;flex:1;min-height:0;overflow:auto;padding-right:4px}
.feed-list::-webkit-scrollbar{width:6px}
.feed-list::-webkit-scrollbar-thumb{background:rgba(148,163,184,.32);border-radius:999px}
.feed-item{display:flex;justify-content:space-between;align-items:center;padding:12px 14px;background:rgba(255,255,255,.58);border:1px solid rgba(226,232,240,.92);border-radius:16px;font-size:12px;color:#3a4a5f;line-height:1.5;box-shadow:0 8px 18px rgba(15,23,42,.03)}
.feed-item b{font-size:12px;font-weight:600;color:#10253f;white-space:nowrap;margin-left:12px}
.split-card{grid-column:span 6;padding:0;overflow:hidden;min-height:460px;display:flex;flex-direction:column}
.panel-head{display:flex;justify-content:space-between;align-items:center;padding:20px 24px 18px;background:linear-gradient(180deg,rgba(255,255,255,.45),rgba(255,255,255,.14));border-bottom:1px solid var(--line)}
.panel-note{font-size:12px;color:var(--muted-soft);letter-spacing:.04em}
.device-body{padding:20px 24px 24px;display:grid;grid-template-columns:1fr 1fr;gap:14px;align-content:start;flex:1}
.device-cell{padding:18px 18px 20px;border:1px solid rgba(226,232,240,.92);border-radius:20px;background:linear-gradient(180deg,rgba(255,255,255,.58) 0,rgba(248,251,255,.9) 100%);box-shadow:0 10px 22px rgba(15,23,42,.03)}
.device-cell b{display:block;margin-bottom:10px;font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted)}
.device-cell span{display:block;font-size:16px;color:#1e293b;line-height:1.5;word-break:break-word}
.table-shell{padding:0 18px 18px;flex:1;overflow:hidden}
.table-wrap{max-height:408px;overflow:auto;border:1px solid rgba(226,232,240,.92);border-radius:20px;background:rgba(255,255,255,.52)}
table{width:100%;border-collapse:collapse;background:transparent}
thead th{position:sticky;top:0;z-index:1;padding:14px 16px;background:rgba(250,252,255,.96);backdrop-filter:blur(12px);text-align:left;font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);border-bottom:1px solid var(--line)}
tbody td{padding:14px 16px;font-size:13px;color:#334155;border-bottom:1px solid rgba(237,242,247,.88);white-space:nowrap}
tbody tr:hover{background:rgba(248,251,255,.9)}
.empty{text-align:center;color:#94a3b8;padding:22px}
.terminal-layout{display:grid;grid-template-columns:minmax(0,1fr) 320px;gap:18px}
.terminal-card{padding:0;overflow:hidden}
.terminal-body{padding:20px}
.shell-log{background:linear-gradient(180deg,#132030 0,#17293c 100%);color:#d7efe8;padding:18px;border-radius:20px;font-family:'Cascadia Mono','SFMono-Regular','SF Mono','JetBrains Mono','Fira Code','DejaVu Sans Mono',monospace;font-size:13px;font-weight:500;-webkit-font-smoothing:antialiased;-moz-osx-font-smoothing:grayscale;text-rendering:geometricPrecision;max-height:calc(100vh - 320px);overflow-y:auto;min-height:420px;border:1px solid rgba(71,85,105,.4);margin-bottom:14px;white-space:pre-wrap;word-wrap:break-word;line-height:1.6;box-shadow:inset 0 1px 0 rgba(255,255,255,.05)}
.shell-prompt{color:#d7efe8}
.shell-userhost{color:#7dd3fc;font-weight:600}
.shell-path{color:#86efac;font-weight:600}
.shell-sign{color:#facc15;font-weight:700}
.shell-command{color:#f8fafc}
.shell-output{color:#d7efe8}
.terminal-input-row{display:flex;gap:10px}
.terminal-input{flex:1;padding:12px 14px;border:1px solid rgba(64,90,116,.8);border-radius:14px;font-family:'Cascadia Mono','SFMono-Regular','SF Mono','JetBrains Mono','Fira Code','DejaVu Sans Mono',monospace;font-size:13px;font-weight:500;-webkit-font-smoothing:antialiased;-moz-osx-font-smoothing:grayscale;text-rendering:geometricPrecision;background:#162434;color:#ecfeff}
.terminal-btn{padding:12px 16px;background:var(--brand);color:#fff;border:none;border-radius:14px;cursor:pointer;font-size:12px;font-weight:600;letter-spacing:.06em;text-transform:uppercase;transition:all .22s ease}
.terminal-btn:hover{background:#193452;transform:translateY(-1px)}
.terminal-btn.secondary{background:#475569}
.terminal-btn.secondary:hover{background:#334155}
.guide-card{padding:20px 22px}
.guide-list{list-style:none;display:flex;flex-direction:column;gap:14px}
.guide-item{padding:14px 14px 16px;border:1px solid rgba(226,232,240,.92);border-radius:18px;background:rgba(255,255,255,.56)}
.guide-item b{display:block;margin-bottom:7px;font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted)}
.guide-item span{display:block;font-size:13px;line-height:1.65;color:#334155}
@media (max-width:1200px){.metric{grid-column:span 6}.health{grid-column:span 12}.feed{grid-column:span 12;height:clamp(260px,30vh,320px);min-height:clamp(260px,30vh,320px)}.split-card{grid-column:span 12}.terminal-layout{grid-template-columns:1fr}.chips{justify-content:flex-start;max-width:none}}
@media (max-width:720px){.shell{padding:16px 14px 28px}.topbar{flex-direction:column}.title-wrap h1{font-size:32px}.device-body{grid-template-columns:1fr}.metric{grid-column:span 12;min-height:156px}.feed{height:300px;min-height:300px}.table-wrap{max-height:420px}.shell-log{min-height:320px;max-height:none}}
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
				document.getElementById('shell-log').innerHTML = renderPrompt(shellPrompt);
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

inline void ReplaceToken(std::string& text, const std::string& token, const std::string& value) {
	std::string::size_type pos = 0;
	while ((pos = text.find(token, pos)) != std::string::npos) {
		text.replace(pos, token.length(), value);
		pos += value.length();
	}
}

inline std::string BuildHtmlPage(const std::string& activePage) {
	std::string html = HTML_TEMPLATE;
	ReplaceToken(html, "__ACTIVE_PAGE__", activePage == "terminal" ? "terminal" : "dashboard");
	return html;
}

}

#endif // __HUI_RENDER_H__
