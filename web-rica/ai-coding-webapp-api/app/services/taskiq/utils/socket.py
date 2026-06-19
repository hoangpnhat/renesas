from typing import Any, Optional
from dataclasses import dataclass, field

import socketio
from loguru import logger

from core.config import settings
from caching.redis import normalize_redis_uri

# Import the channel name from main socket module for consistency
from api.v1.socket import SOCKETIO_CHANNEL


@dataclass
class SocketHelper:
    """
    Socket helper for emitting messages from background tasks (taskiq workers).

    Uses the same Redis channel as the main socket server to enable cross-process
    communication. Background tasks can emit events to connected clients without
    direct access to the main sio instance.

    Usage:
        async with SocketHelper() as socket:
            await socket.emit("event_name", {"data": "value"}, to=sid)

        # Or manual management:
        socket = SocketHelper()
        await socket.connect()
        await socket.emit("event_name", {"data": "value"})
        await socket.disconnect()
    """

    _manager: Optional[socketio.AsyncRedisManager] = field(default=None, init=False)
    _connected: bool = field(default=False, init=False)

    @property
    def redis_url(self) -> str:
        assert settings.REDIS_URI is not None
        return normalize_redis_uri(settings.REDIS_URI)

    async def connect(self) -> "SocketHelper":
        if self._connected:
            return self

        try:
            self._manager = socketio.AsyncRedisManager(
                self.redis_url,
                write_only=True,
                channel=SOCKETIO_CHANNEL
            )
            
            self._connected = True
            logger.debug("SocketHelper connected to Redis manager")
        except Exception as e:
            logger.error(f"Failed to connect SocketHelper: {e}")
            raise

        return self

    async def disconnect(self) -> None:
        if self._manager and self._connected:
            try:
                self._manager = None
                self._connected = False
                logger.debug("SocketHelper disconnected")
            except Exception as e:
                logger.error(f"Error disconnecting SocketHelper: {e}")

    async def emit(
        self,
        event: str,
        data: Any,
        to: Optional[str] = None,
        room: Optional[str] = None,
        namespace: Optional[str] = None,
        skip_sid: Optional[str] = None,
    ) -> bool:
        """
        Emit an event to connected socket clients.

        Args:
            event: Event name (e.g., "file_status", "progress_update")
            data: Data payload to send (will be JSON serialized)
            to: Target session ID (sid) for direct message
            room: Target room name for room broadcast
            namespace: Socket.IO namespace (default: "/")
            skip_sid: Session ID to skip when broadcasting

        Returns:
            bool: True if emit was successful, False otherwise
        """
        if not self._connected or not self._manager:
            logger.warning("SocketHelper not connected. Call connect() first.")
            return False

        try:
            await self._manager.emit(
                event,
                data=data,
                to=to,
                room=room,
                namespace=namespace or "/",
                skip_sid=skip_sid,
            )
            logger.debug(f"Emitted event '{event}' to={to or room or 'all'}")
            return True
        except Exception as e:
            logger.error(f"Failed to emit event '{event}': {e}")
            return False

    async def emit_to_user(
        self,
        event: str,
        data: Any,
        sid: str,
    ) -> bool:
        """
        Convenience method to emit directly to a specific user session.

        Args:
            event: Event name
            data: Data payload
            sid: Target session ID
        """
        return await self.emit(event, data, to=sid)

    async def broadcast(
        self,
        event: str,
        data: Any,
        room: Optional[str] = None,
        skip_sid: Optional[str] = None,
    ) -> bool:
        """
        Broadcast an event to all clients or a specific room.

        Args:
            event: Event name
            data: Data payload
            room: Optional room to broadcast to
            skip_sid: Optional session ID to exclude
        """
        return await self.emit(event, data, room=room, skip_sid=skip_sid)

    async def __aenter__(self) -> "SocketHelper":
        """Async context manager entry."""
        await self.connect()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb) -> None:
        """Async context manager exit."""
        await self.disconnect()


async def emit_socket_event(
    event: str,
    data: Any,
    to: Optional[str] = None,
    room: Optional[str] = None,
) -> bool:
    """
    Quick helper to emit a single socket event from background tasks.

    Usage:
        await emit_socket_event("file_processed", {"file_id": "123"}, to=user_sid)
    """
    async with SocketHelper() as socket:
        return await socket.emit(event, data, to=to, room=room)
