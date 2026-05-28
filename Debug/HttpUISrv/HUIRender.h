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

static const std::string HTML_PAGE =
R"__HUI__(<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>Sparrow Device Live Dashboard</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{
	--bg-1:#f2f6ff;
	--bg-2:#e8f4f2;
	--ink:#1b2533;
	--muted:#5e6b7c;
	--panel:#ffffff;
	--line:#e3eaf3;
	--brand:#0d9488;
	--brand-strong:#0f766e;
	--ok:#2e7d32;
	--warn:#ed6c02;
	--danger:#c62828;
}
html,body{width:100%;height:100%;font-family:'Segoe UI',Roboto,sans-serif;color:var(--ink);background:radial-gradient(circle at 10% 10%,#dff3ff 0,var(--bg-1) 40%,var(--bg-2) 100%)}
.shell{min-height:100vh;padding:22px 24px 28px}
.topbar{display:flex;justify-content:space-between;align-items:flex-end;gap:16px;margin-bottom:18px}
.title-wrap h1{font-size:30px;letter-spacing:.2px}
.subtitle{margin-top:6px;color:var(--muted);font-size:14px}
.chips{display:flex;flex-wrap:wrap;gap:10px}
.chip{display:flex;align-items:center;gap:8px;padding:7px 12px;border-radius:999px;background:#fff;border:1px solid var(--line);font-size:13px;color:#334155}
.dot{width:9px;height:9px;border-radius:50%;background:#94a3b8;transition:all .2s ease}
.dot.ok{background:var(--ok);box-shadow:0 0 0 4px rgba(46,125,50,.15)}
.dot.warn{background:var(--warn);box-shadow:0 0 0 4px rgba(237,108,2,.12)}
.dot.err{background:var(--danger);box-shadow:0 0 0 4px rgba(198,40,40,.15)}
.chip.subtle-note{font-size:12px;color:#8a6b21;background:#fff8e8;border-color:#f2e4be;opacity:.82;display:none}
.chip.subtle-note.show{display:flex}
.grid{display:grid;grid-template-columns:repeat(12,minmax(0,1fr));gap:14px}
.card{background:var(--panel);border:1px solid var(--line);border-radius:14px;box-shadow:0 6px 20px rgba(17,24,39,.04)}
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
.queues{grid-column:span 12;padding:0;overflow:hidden}
.queues-head{display:flex;justify-content:space-between;align-items:center;padding:14px 18px;background:#f8fbff;border-bottom:1px solid var(--line)}
.queues-note{font-size:12px;color:#64748b}
table{width:100%;border-collapse:collapse}
thead th{padding:12px 14px;background:#fbfdff;text-align:left;font-size:12px;text-transform:uppercase;letter-spacing:.6px;color:#64748b;border-bottom:1px solid var(--line)}
tbody td{padding:12px 14px;font-size:13px;color:#334155;border-bottom:1px solid #edf2f7}
tbody tr:hover{background:#f8fbff}
.empty{text-align:center;color:#94a3b8;padding:18px}
.shell-log{background:#1b2533;color:#d7efe8;padding:12px;border-radius:4px;font-family:monospace;font-size:11px;max-height:300px;overflow-y:auto;min-height:120px;border:1px solid var(--line);margin-bottom:12px;white-space:pre-wrap;word-wrap:break-word;line-height:1.4}
.shell-prompt{color:#d7efe8}
.shell-userhost{color:#7dd3fc;font-weight:600}
.shell-path{color:#86efac;font-weight:600}
.shell-sign{color:#facc15;font-weight:700}
.shell-command{color:#f8fafc}
.shell-output{color:#d7efe8}
@media (max-width:1200px){.metric{grid-column:span 6}.health{grid-column:span 12}.feed{grid-column:span 12}}
@media (max-width:700px){.shell{padding:14px}.topbar{flex-direction:column;align-items:flex-start}.metric{grid-column:span 12}}
</style>
</head>
<body>
<div class='shell'>
	<div class='topbar'>
		<div class='title-wrap'>
			<h1>Device Live Dashboard</h1>
			<div class='subtitle'>Realtime status stream for current Sparrow device</div>
		</div>
		<div class='chips'>
			<div class='chip'><span id='conn-dot' class='dot'></span><span id='conn-text'>Connecting</span></div>
			<div class='chip'>Host: <span id='chip-host'>-</span></div>
			<div class='chip'>Last Refresh: <span id='last-refresh'>-</span></div>
			<div class='chip'>Latency: <span id='req-latency'>-</span></div>
			<div id='data-note' class='chip subtle-note'>Showing last valid snapshot</div>
		</div>
	</div>

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

		<div class='card queues'>
			<div class='queues-head'>
				<div class='sec-title' style='margin:0'>Queue Snapshot (sub-module)</div>
				<div class='queues-note'>Queue data is only one part of device state</div>
			</div>
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

	<div class='card'>
		<div class='queues-head'>
			<div class='sec-title' style='margin:0'>Device Profile</div>
			<div class='queues-note'>System information</div>
		</div>
		<div style='padding:16px;display:grid;grid-template-columns:1fr 1fr;gap:12px'>
			<div><b>Hostname:</b><br><span id='profile-hostname'>-</span></div>
			<div><b>Uptime:</b><br><span id='profile-uptime'>-</span></div>
			<div><b>Kernel:</b><br><span id='profile-kernel'>-</span></div>
			<div id='profile-resources'><b>Memory/Disk:</b><br><span>-</span></div>
		</div>
	</div>

	<div class='card'>
		<div class='queues-head'>
			<div class='sec-title' style='margin:0'>Remote Terminal</div>
			<div class='queues-note'>Interactive shell like SSH</div>
		</div>
		<div style='padding:16px'>
			<div id='shell-log' class='shell-log'></div>
			<div style='display:flex;gap:8px'>
				<input type='text' id='shell-input' placeholder='Enter command...' style='flex:1;padding:6px 10px;border:1px solid var(--line);border-radius:4px;font-family:monospace;font-size:11px;background:#1b2533;color:#0d9488'>
				<button id='shell-exec-btn' onclick='executeShellCmd()' style='padding:6px 12px;background:var(--brand);color:white;border:none;border-radius:4px;cursor:pointer;font-size:11px'>Send</button>
			</div>
		</div>
	</div>
</div>

<script>
const HUI_REFRESH_INTERVAL_MS = 2000;
const HUI_FEED_LIMIT = 12;
const feedHistory = [];
let hasValidSnapshot = false;

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
		document.getElementById('profile-resources').innerHTML = '<b>Memory/Disk:</b><br><span>' + (resources.memory || '-') + ' / ' + (resources.disk || '-') + '</span>';
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

function executeShellCmd() {
	const input = document.getElementById('shell-input');
	const cmd = input.value.trim();
	if (!cmd) {
		return;  // Don't show alert, just ignore empty input
	}

	const log = document.getElementById('shell-log');
	const encodedCmd = encodeURIComponent(cmd);

	fetch('/api/shell?cmd=' + encodedCmd)
		.then(function (r) { return r.json(); })
		.then(function (data) {
			if (data.error) {
				appendShellHtml(renderOutput(data.error) + '<br>');
			} else {
				let html = '';
				if (data.promptBefore) {
					html += renderPrompt(data.promptBefore);
				}
				if (data.command) {
					html += '<span class="shell-command">' + escapeHtml(data.command) + '</span><br>';
				}
				if (data.output) {
					html += renderOutput(data.output);
					if (!String(data.output).endsWith('\n')) {
						html += '<br>';
					}
				}
				appendShellHtml(html);
			}

			// Clear input for next command.
			input.value = '';
			input.focus();
		})
		.catch(function (e) {
			appendShellHtml(renderOutput('Request failed: ' + e.message) + '<br>');
			input.focus();
		});
}

function initShellTerminal() {
	// Initialize shell terminal by fetching current prompt once.
	fetch('/api/shell?cmd=')
		.then(function (r) { return r.json(); })
		.then(function (data) {
			const prompt = data.prompt || data.promptAfter;
			if (prompt) {
				document.getElementById('shell-log').innerHTML = renderOutput('Welcome to Remote Terminal') + '<br><br>' + renderPrompt(prompt) + '<br>';
			}
		})
		.catch(function (e) {
			console.log('Init shell terminal failed: ' + e.message);
		});

	// Allow Enter key to execute command
	document.getElementById('shell-input').addEventListener('keydown', function (e) {
		if (e.key === 'Enter') {
			executeShellCmd();
		}
	});

	document.getElementById('shell-input').focus();
}

document.getElementById('chip-host').textContent = window.location.host || '-';
setInterval(loadData, HUI_REFRESH_INTERVAL_MS);
loadData();
initShellTerminal();
</script>
</body>
</html>)__HUI__";

}

#endif // __HUI_RENDER_H__
