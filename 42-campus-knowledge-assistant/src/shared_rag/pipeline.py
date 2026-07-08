from __future__ import annotations

from dataclasses import dataclass
import os
from typing import Iterable


@dataclass
class Chunk:
    id: str
    source: str
    text: str


class SimpleChunker:
    def __init__(self, chunk_size: int = 420, overlap: int = 60) -> None:
        if overlap >= chunk_size:
            raise ValueError("overlap must be smaller than chunk_size")
        self.chunk_size = chunk_size
        self.overlap = overlap

    def chunk(self, source: str, text: str) -> list[Chunk]:
        words = text.split()
        if not words:
            return []
        chunks: list[Chunk] = []
        step = self.chunk_size - self.overlap
        index = 0
        while index < len(words):
            piece = words[index : index + self.chunk_size]
            chunk_text = " ".join(piece).strip()
            if chunk_text:
                chunks.append(Chunk(id=f"{source}:{len(chunks)}", source=source, text=chunk_text))
            index += step
        return chunks


class RAGPipeline:
    """Shared RAG pipeline used by both hackathon prototypes."""

    def __init__(self, collection_name: str, chunk_size: int = 420, overlap: int = 60) -> None:
        self.chunker = SimpleChunker(chunk_size=chunk_size, overlap=overlap)
        self.collection_name = collection_name
        self._chunks: list[Chunk] = []
        self._metadatas: list[dict[str, str]] = []

        self._embedder = None
        self._vector_store = None
        self._use_chroma = False

    def _lazy_init(self) -> None:
        if self._embedder is not None:
            return
        from sentence_transformers import SentenceTransformer

        self._embedder = SentenceTransformer("sentence-transformers/all-MiniLM-L6-v2")

        try:
            import chromadb

            client = chromadb.Client()
            self._vector_store = client.create_collection(name=self.collection_name)
            self._use_chroma = True
        except Exception:
            self._vector_store = None
            self._use_chroma = False

    def ingest(self, docs: Iterable[tuple[str, str]]) -> int:
        self._lazy_init()
        self._chunks.clear()
        for source, content in docs:
            self._chunks.extend(self.chunker.chunk(source=source, text=content))

        if not self._chunks:
            return 0

        texts = [chunk.text for chunk in self._chunks]
        embeddings = self._embedder.encode(texts).tolist()
        self._metadatas = [{"source": chunk.source, "chunk_id": chunk.id} for chunk in self._chunks]

        if self._use_chroma and self._vector_store is not None:
            self._vector_store.add(
                ids=[chunk.id for chunk in self._chunks],
                documents=texts,
                metadatas=self._metadatas,
                embeddings=embeddings,
            )
        else:
            for idx, metadata in enumerate(self._metadatas):
                metadata["embedding"] = embeddings[idx]
        return len(self._chunks)

    def retrieve(self, query: str, top_k: int = 4) -> list[dict[str, str]]:
        if not self._chunks:
            return []
        self._lazy_init()
        query_embedding = self._embedder.encode([query])[0]

        if self._use_chroma and self._vector_store is not None:
            result = self._vector_store.query(query_embeddings=[query_embedding.tolist()], n_results=top_k)
            docs = result.get("documents", [[]])[0]
            metas = result.get("metadatas", [[]])[0]
            return [
                {
                    "source": metas[i].get("source", "unknown"),
                    "snippet": docs[i],
                }
                for i in range(len(docs))
            ]

        scored: list[tuple[float, int]] = []
        for i, metadata in enumerate(self._metadatas):
            emb = metadata.get("embedding")
            if emb is None:
                continue
            score = self._cosine_similarity(query_embedding, emb)
            scored.append((score, i))

        scored.sort(reverse=True, key=lambda item: item[0])
        results: list[dict[str, str]] = []
        for _, idx in scored[:top_k]:
            results.append(
                {
                    "source": self._metadatas[idx]["source"],
                    "snippet": self._chunks[idx].text,
                }
            )
        return results

    @staticmethod
    def _cosine_similarity(vec_a, vec_b) -> float:
        dot = sum(a * b for a, b in zip(vec_a, vec_b))
        mag_a = sum(a * a for a in vec_a) ** 0.5
        mag_b = sum(b * b for b in vec_b) ** 0.5
        if mag_a == 0 or mag_b == 0:
            return 0.0
        return dot / (mag_a * mag_b)


def load_text_files(data_dir: str) -> list[tuple[str, str]]:
    docs: list[tuple[str, str]] = []
    for name in sorted(os.listdir(data_dir)):
        if not name.endswith((".txt", ".md")):
            continue
        path = os.path.join(data_dir, name)
        with open(path, "r", encoding="utf-8") as f:
            docs.append((name, f.read()))
    return docs
